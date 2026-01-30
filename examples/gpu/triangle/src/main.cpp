// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <csignal>

import std;

import stormkit;
import gpu_app;

#include <stormkit/core/try_expected.hpp>
#include <stormkit/log/log_macro.hpp>
#include <stormkit/main/main_macro.hpp>

LOGGER("stormkit.examples.gpu.triangle");

#ifndef SHADER_DIR
    #define SHADER_DIR "."
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
    gpu::Semaphore        render_finished;
};

static constexpr auto BUFFERING_COUNT = 2;

class Application: public base::Application {
  public:
    auto init_example() {
        // load shaders
        m_vertex_shader = TryAssert(gpu::Shader::load_from_file(m_device,
                                                                SHADER_DIR "/shaders/triangle.spv",
                                                                gpu::ShaderStageFlag::VERTEX),
                                    std::format("Failed to load vertex shader {}", SHADER_DIR "/shaders/triangle.spv"));

        m_fragment_shader = TryAssert(gpu::Shader::load_from_file(m_device,
                                                                  SHADER_DIR "/shaders/triangle.spv",
                                                                  gpu::ShaderStageFlag::FRAGMENT),
                                      std::format("Failed to load fragment shader {}", SHADER_DIR "/shaders/triangle.spv"));

        m_pipeline_layout = TryAssert(gpu::PipelineLayout::create(m_device, {}), "Failed to create pipeline layout");

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

        const auto rendering_info = gpu::RasterPipelineRenderingInfo {
            .color_attachment_formats = { m_swapchain->pixel_format() }
        };

        m_pipeline = TryAssert(gpu::Pipeline::create(m_device, state, m_pipeline_layout, rendering_info),
                               "Failed to create raster pipeline");

        // create present engine resources
        m_submission_resources = init_by<std::vector<SubmissionResource>>([&](auto& out) noexcept {
            out.reserve(BUFFERING_COUNT);
            for (auto _ : range(BUFFERING_COUNT)) {
                out.push_back({
                  .in_flight       = TryAssert(gpu::Fence::create_signaled(m_device),
                                               "Failed to create swapchain image "
                                               "in flight fence"),
                  .image_available = TryAssert(gpu::Semaphore::create(m_device),
                                               "Failed to create "
                                               "present wait semaphore"),
                  .render_cmb      = TryAssert(m_command_pool->create_command_buffer(),
                                               "Failed to create transition "
                                               "command buffers"),
                });
            }
        });

        // transition swapchain image to present image
        const auto& images = m_swapchain->images();

        const auto image_count     = stdr::size(images);
        auto       transition_cmbs = TryAssert(m_command_pool->create_command_buffers(image_count),
                                               "Failed to create transition command buffers");
        m_image_resources.reserve(stdr::size(images));

        auto image_index = 0u;
        for (const auto& swap_image : images) {
            auto view = TryAssert(gpu::ImageView::create(m_device, swap_image), "Failed to create swapchain image view");

            m_image_resources
              .push_back({ .image           = as_ref(swap_image),
                           .view            = std::move(view),
                           .render_finished = TryAssert(gpu::Semaphore::create(m_device),
                                                        "Failed to create render "
                                                        "signal semaphore") });

            auto& transition_cmb = transition_cmbs[image_index];
            TryAssertDiscard(transition_cmb.begin(true), "Failed to begin texture transition command buffer");

            transition_cmb.begin_debug_region(std::format("transition image {}", image_index))
              .transition_image_layout(swap_image, gpu::ImageLayout::UNDEFINED, gpu::ImageLayout::PRESENT_SRC)
              .end_debug_region();

            TryAssertDiscard(transition_cmb.end(),
                             "Failed to begin texture transition command "
                             "buffer");

            ++image_index;
        }

        const auto fence = TryAssert(gpu::Fence::create(m_device), "Failed to create transition fence");

        const auto cmbs = to_refs(transition_cmbs);

        TryAssert(m_raster_queue->submit({ .command_buffers = cmbs }, as_ref(fence)),
                  "Failed to submit texture transition command buffers");

        // wait for transition to be done
        TryAssert(fence.wait(), "");
    }

    auto run_example() {
        LOG_MODULE.flush();

        // get next swapchain image
        auto& submission_resource = m_submission_resources[m_current_frame];

        const auto& wait      = submission_resource.image_available;
        auto&       in_flight = submission_resource.in_flight;

        TryAssert(in_flight.wait(), "Failed to wait in_flight fence");
        in_flight.reset();

        const auto&& [_, image_index] = TryAssert(m_swapchain->acquire_next_image(100ms, wait),
                                                  "Failed to acquire next swapchain image");

        const auto& swapchain_image_resource = m_image_resources[image_index];
        const auto& signal                   = swapchain_image_resource.render_finished;

        static constexpr auto PIPELINE_FLAGS = std::array { gpu::PipelineStageFlag::COLOR_ATTACHMENT_OUTPUT };

        // render in it
        auto& render_cmb = submission_resource.render_cmb;
        TryAssertDiscard(render_cmb.reset(), "Failed to reset render command buffer");
        TryAssertDiscard(render_cmb.begin(), "Failed to begin render command buffer");

        const auto window_extent  = m_window->extent().to<i32>();
        const auto rendering_info = gpu::RenderingInfo {
            .render_area       = { .x = 0, .y = 0, .width = window_extent.width, .height = window_extent.height },
            .color_attachments = { { .image_view  = as_ref(swapchain_image_resource.view),
                                     .layout      = gpu::ImageLayout::ATTACHMENT_OPTIMAL,
                                     .clear_value = gpu::ClearColor { .color = RGBColorDef::SILVER<float> } } }
        };

        render_cmb
          .transition_image_layout(swapchain_image_resource.image,
                                   gpu::ImageLayout::PRESENT_SRC,
                                   gpu::ImageLayout::ATTACHMENT_OPTIMAL)
          .begin_debug_region("Render triangle")
          .begin_rendering(rendering_info)
          .bind_pipeline(m_pipeline)
          .draw(3)
          .end_rendering()
          .end_debug_region()
          .transition_image_layout(swapchain_image_resource.image,
                                   gpu::ImageLayout::ATTACHMENT_OPTIMAL,
                                   gpu::ImageLayout::PRESENT_SRC);

        TryAssertDiscard(render_cmb.end(), "Failed to end render command buffer");
        TryAssertDiscard(render_cmb.submit(m_raster_queue, as_refs(wait), PIPELINE_FLAGS, as_refs(signal), as_ref(in_flight)),
                         "Failed to submit render command buffer");

        // present it
        TryAssert(m_raster_queue->present(as_refs(m_swapchain), as_refs(signal), as_view(image_index)),
                  "Failed to present swapchain image");

        if (++m_current_frame >= BUFFERING_COUNT) m_current_frame = 0;
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
