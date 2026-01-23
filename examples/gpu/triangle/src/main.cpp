// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

import std;

import stormkit;
import gpu_app;

#include <stormkit/log/log_macro.hpp>
#include <stormkit/main/main_macro.hpp>

LOGGER("stormkit.examples.gpu.triangle");

#ifndef SHADER_DIR
    #define SHADER_DIR "../share/shaders"
#endif

namespace stdr  = std::ranges;
namespace stdfs = std::filesystem;

using namespace std::literals;
using namespace stormkit;

struct SubmissionResource {
    gpu::Fence         in_flight;
    gpu::Semaphore     image_available;
    gpu::CommandBuffer render_cmb;
};

struct SwapchainImageResource {
    Ref<const gpu::Image> image;
    gpu::ImageView        view;
    gpu::FrameBuffer      framebuffer;
    gpu::Semaphore        render_finished;
};

static constexpr auto BUFFERING_COUNT = 2;

class Application: public base::Application {
  public:
    auto init_example() {
        // load shaders
        m_vertex_shader = gpu::Shader::load_from_file(m_device, SHADER_DIR "/triangle.spv", gpu::ShaderStageFlag::VERTEX)
                            .transform_error(monadic::assert("Failed to load vertex shader"))
                            .value();

        m_fragment_shader = gpu::Shader::load_from_file(m_device, SHADER_DIR "/triangle.spv", gpu::ShaderStageFlag::FRAGMENT)
                              .transform_error(monadic::assert("Failed to load fragment shader"))
                              .value();

        m_pipeline_layout = gpu::PipelineLayout::create(m_device, {})
                              .transform_error(monadic::assert("Failed to create pipeline layout"))
                              .value();

        // initialize render pass
        m_render_pass = gpu::RenderPass::create(m_device,
                                                { .attachments = { { .format = m_swapchain->pixel_format() } },
                                                  .subpasses   = { { .bind_point            = gpu::PipelineBindPoint::GRAPHICS,
                                                                     .color_attachment_refs = { { .attachment_id = 0u } } } } })
                          .transform_error(monadic::assert("Failed to create render pass"))
                          .value();

        const auto window_extent = m_window->extent();

        // initialize render pipeline
        const auto window_viewport = gpu::Viewport {
            .position = { 0.f, 0.f },
            .extent   = window_extent.to<f32>(),
            .depth    = { 0.f, 1.f },
        };
        const auto scissor = gpu::Scissor {
            .offset = { 0, 0 },
            .extent = window_extent,
        };

        const auto state = gpu::RasterPipelineState {
        .input_assembly_state = { .topology = gpu::PrimitiveTopology::TRIANGLE_LIST, },
        .viewport_state       = { .viewports = { window_viewport },
                                 .scissors  = { scissor }, },
        .color_blend_state
        = { .attachments = { { .blend_enable           = true,
                               .src_color_blend_factor = gpu::BlendFactor::SRC_ALPHA,
                               .dst_color_blend_factor = gpu::BlendFactor::ONE_MINUS_SRC_ALPHA,
                               .src_alpha_blend_factor = gpu::BlendFactor::SRC_ALPHA,
                               .dst_alpha_blend_factor = gpu::BlendFactor::ONE_MINUS_SRC_ALPHA,
                               .alpha_blend_operation  = gpu::BlendOperation::ADD, }, }, },
        .shader_state  = to_refs(m_vertex_shader, m_fragment_shader),
    };

        m_pipeline = gpu::Pipeline::create(m_device, state, m_pipeline_layout, m_render_pass)
                       .transform_error(monadic::assert("Failed to create raster pipeline"))
                       .value();

        // create present engine resources
        m_submission_resources = init_by<std::vector<SubmissionResource>>([&](auto& out) noexcept {
            out.reserve(BUFFERING_COUNT);
            for (auto _ : range(BUFFERING_COUNT)) {
                out.push_back({
                  .in_flight = gpu::Fence::create_signaled(m_device)
                                 .transform_error(monadic::assert("Failed to create swapchain image "
                                                                  "in flight fence"))
                                 .value(),
                  .image_available = gpu::Semaphore::create(m_device)
                                       .transform_error(monadic::assert("Failed to create "
                                                                        "present wait semaphore"))
                                       .value(),
                  .render_cmb = m_command_pool->create_command_buffer()
                                  .transform_error(monadic::assert("Failed to create transition "
                                                                   "command buffers"))
                                  .value(),
                });
            }
        });

        // transition swapchain image to present image
        const auto& images = m_swapchain->images();

        const auto image_count     = stdr::size(images);
        auto       transition_cmbs = m_command_pool->create_command_buffers(image_count)
                                 .transform_error(monadic::assert("Failed to create transition command buffers"))
                                 .value();
        m_image_resources.reserve(stdr::size(images));

        auto image_index = 0u;
        for (const auto& swap_image : images) {
            auto view = gpu::ImageView::create(m_device, swap_image)
                          .transform_error(core::monadic::assert("Failed to create swapchain image view"))
                          .value();
            auto framebuffer = m_render_pass->create_frame_buffer(m_device, window_extent, to_refs(view))
                                 .transform_error(core::monadic::assert(std::format("Failed to create framebuffer for image {}",
                                                                                    image_index)))
                                 .value();

            m_image_resources.push_back({
              .image           = as_ref(swap_image),
              .view            = std::move(view),
              .framebuffer     = std::move(framebuffer),
              .render_finished = gpu::Semaphore::create(m_device)
                                   .transform_error(core::monadic::assert("Failed to create render "
                                                                          "signal semaphore"))
                                   .value(),
            });

            auto& transition_cmb = transition_cmbs[image_index];
            *transition_cmb.begin(true)
               .transform_error(monadic::assert("Failed to begin texture transition command buffer"))
               .value()
               ->begin_debug_region(std::format("transition image {}", image_index))
               .transition_image_layout(swap_image, gpu::ImageLayout::UNDEFINED, gpu::ImageLayout::PRESENT_SRC)
               .end_debug_region()
               .end()
               .transform_error(monadic::assert("Failed to begin texture transition command "
                                                "buffer"));

            ++image_index;
        }

        const auto fence = gpu::Fence::create(m_device)
                             .transform_error(monadic::assert("Failed to create transition fence"))
                             .value();

        const auto cmbs = to_refs(transition_cmbs);

        m_raster_queue->submit({ .command_buffers = cmbs }, as_ref(fence))
          .transform_error(monadic::assert("Failed to submit texture transition command buffers"))
          .value();

        // wait for transition to be done
        fence.wait().transform_error(monadic::assert());
    }

    auto run_example() {
        LOG_MODULE.flush();

        // get next swapchain image
        auto& submission_resource = m_submission_resources[m_current_frame];

        const auto& wait      = submission_resource.image_available;
        auto&       in_flight = submission_resource.in_flight;

        const auto acquire_next_image = bind_front(&gpu::SwapChain::acquire_next_image, &*m_swapchain, 100ms, std::cref(wait));
        const auto extract_index      = [](auto&& _result) static noexcept {
            auto&& [result, _image_index] = _result;
            return _image_index;
        };

        const auto image_index = in_flight.wait()
                                   .transform([&in_flight](auto&&) mutable noexcept { in_flight.reset(); })
                                   .and_then(acquire_next_image)
                                   .transform(extract_index)
                                   .transform_error(monadic::assert("Failed to acquire next swapchain image"))
                                   .value();

        const auto& swapchain_image_resource = m_image_resources[image_index];
        const auto& framebuffer              = swapchain_image_resource.framebuffer;
        const auto& signal                   = swapchain_image_resource.render_finished;

        static constexpr auto PIPELINE_FLAGS = std::array { gpu::PipelineStageFlag::COLOR_ATTACHMENT_OUTPUT };

        // render in it
        auto& render_cmb = submission_resource.render_cmb;
        *render_cmb.reset()
           .transform_error(monadic::assert("Failed to reset render command buffer"))
           .value()
           ->begin()
           .transform_error(monadic::assert("Failed to begin render command buffer"))
           .value()
           ->begin_debug_region("Render triangle")
           .begin_render_pass(m_render_pass, framebuffer)
           .bind_pipeline(m_pipeline)
           .draw(3)
           .end_render_pass()
           .end_debug_region()
           .end()
           .transform_error(monadic::assert("Failed to end render command buffer"))
           .value()
           ->submit(m_raster_queue, as_refs(wait), PIPELINE_FLAGS, as_refs(signal), as_ref(in_flight))
           .transform_error(monadic::assert("Failed to submit render command buffer"));

        // present it
        auto update_current_frame = [this](auto&&) mutable noexcept {
            if (++m_current_frame >= BUFFERING_COUNT) m_current_frame = 0;
        };

        m_raster_queue->present(as_refs(m_swapchain), as_refs(signal), as_view(image_index))
          .transform(update_current_frame)
          .transform_error(monadic::assert("Failed to present swapchain image"));
    }

    constexpr auto example_name() const noexcept -> std::string_view { return "Triangle"; }

  private:
    DeferInit<gpu::Shader>              m_vertex_shader;
    DeferInit<gpu::Shader>              m_fragment_shader;
    DeferInit<gpu::PipelineLayout>      m_pipeline_layout;
    DeferInit<gpu::RenderPass>          m_render_pass;
    DeferInit<gpu::Pipeline>            m_pipeline;
    std::vector<SubmissionResource>     m_submission_resources;
    std::vector<SwapchainImageResource> m_image_resources;
    usize                               m_current_frame = 0_usize;
};

auto main(std::span<const std::string_view> args) -> int {
    auto app = Application {};
    app.run(args);
    return 0;
}
