// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

import std;

import stormkit;
import gpu_app;

#include <stormkit/gpu/vulkan.hpp>
#include <stormkit/log/log_macro.hpp>
#include <stormkit/main/main_macro.hpp>

#define IMGUI_IMPL_VULKAN_NO_PROTOTYPES
#include <imgui.h>
#include <imgui_impl_vulkan.h>
#undef assert

LOGGER("stormkit.examples.gpu.imgui");

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
        init_resources();
        init_imgui();
    }

    auto init_resources() -> void {
        // initialilze descriptor pool
        static constexpr auto POOL_SIZES = std::array {
            gpu::DescriptorPool::Size { .type = gpu::DescriptorType::COMBINED_IMAGE_SAMPLER, .descriptor_count = BUFFERING_COUNT }
        };
        m_descriptor_pool = gpu::DescriptorPool::create(m_device, POOL_SIZES, BUFFERING_COUNT)
                              .transform_error(monadic::assert("Failed to create descriptor pool"))
                              .value();

        // initialize render pass
        m_render_pass = gpu::RenderPass::create(m_device,
                                                { .attachments = { { .format = m_swapchain->pixel_format() } },
                                                  .subpasses   = { { .bind_point            = gpu::PipelineBindPoint::GRAPHICS,
                                                                     .color_attachment_refs = { { .attachment_id = 0u } } } } })
                          .transform_error(monadic::assert("Failed to create render pass"))
                          .value();

        const auto window_extent = m_window->extent();

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

    auto init_imgui() -> void {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io      = ImGui::GetIO();
        io.DisplaySize.x = m_window->extent().to<f32>().width;
        io.DisplaySize.y = m_window->extent().to<f32>().height;

        /*const*/ auto init_info = ImGui_ImplVulkan_InitInfo {
            .ApiVersion                  = VK_API_VERSION_1_1,
            .Instance                    = m_instance->native_handle(),
            .PhysicalDevice              = m_physical_device->native_handle(),
            .Device                      = m_device->native_handle(),
            .QueueFamily                 = 0,
            .Queue                       = m_raster_queue->native_handle(),
            .DescriptorPool              = m_descriptor_pool->native_handle(),
            .DescriptorPoolSize          = 0,
            .MinImageCount               = BUFFERING_COUNT,
            .ImageCount                  = BUFFERING_COUNT,
            .PipelineCache               = nullptr,
            .PipelineInfoMain = {
            .RenderPass                  = m_render_pass->native_handle(),
            .Subpass                     = 0,
            .MSAASamples                 = VK_SAMPLE_COUNT_1_BIT,
            .PipelineRenderingCreateInfo = {},
          },
            .UseDynamicRendering         = false,
            .Allocator                   = nullptr,
            .CheckVkResultFn =
              [](auto result) static noexcept {
                  if (result != VK_SUCCESS) elog("{}", gpu::from_vk<gpu::Result>(result));
              },
            .MinAllocationSize = 1024 * 1024,
            .CustomShaderVertCreateInfo = {},
            .CustomShaderFragCreateInfo = {},
        };
        ImGui_ImplVulkan_LoadFunctions(VK_API_VERSION_1_1, gpu::imgui_vk_loader, &*m_device);
        ImGui_ImplVulkan_Init(&init_info);

        m_window
          ->on(wsi::KeyDownEventFunc { [this, &io](u8 /*id*/, wsi::Key key, char c) mutable noexcept {
                   if (key == wsi::Key::ESCAPE) m_window->close();
                   io.AddInputCharactersUTF8(&c);
               } },
               wsi::MouseMovedEventFunc { [&io](u8 /*id*/, const math::vec2i& position) mutable noexcept {
                   const auto _position = position.to<f32>();

                   io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
                   io.AddMousePosEvent(_position.x, _position.y);
               } },
               wsi::MouseButtonDownEventFunc { [&io](u8 /*id*/, wsi::MouseButton button, const math::vec2i&) mutable noexcept {
                   auto mouse_button = -1;
                   if (button == wsi::MouseButton::LEFT) mouse_button = 0;
                   if (button == wsi::MouseButton::RIGHT) mouse_button = 1;
                   if (button == wsi::MouseButton::MIDDLE) mouse_button = 2;
                   if (button == wsi::MouseButton::BUTTON_1) mouse_button = 3;
                   if (button == wsi::MouseButton::BUTTON_2) mouse_button = 4;
                   if (mouse_button == -1) return;
                   io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
                   io.AddMouseButtonEvent(mouse_button, true);
               } },
               wsi::MouseButtonUpEventFunc { [&io](u8 /*id*/, wsi::MouseButton button, const math::vec2i&) mutable noexcept {
                   auto mouse_button = -1;
                   if (button == wsi::MouseButton::LEFT) mouse_button = 0;
                   if (button == wsi::MouseButton::RIGHT) mouse_button = 1;
                   if (button == wsi::MouseButton::MIDDLE) mouse_button = 2;
                   if (button == wsi::MouseButton::BUTTON_1) mouse_button = 3;
                   if (button == wsi::MouseButton::BUTTON_2) mouse_button = 4;
                   if (mouse_button == -1) return;
                   io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
                   io.AddMouseButtonEvent(mouse_button, false);
               } });
    }

    auto run_example() {
        LOG_MODULE.flush();

        ImGui_ImplVulkan_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();
        ImGui::Render();

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
        render_cmb.reset()
          .transform_error(monadic::assert("Failed to reset render command buffer"))
          .value()
          ->begin()
          .transform_error(monadic::assert("Failed to begin render command buffer"))
          .value()
          ->begin_debug_region("Render imgui")
          .begin_render_pass(m_render_pass, framebuffer);

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), render_cmb.native_handle());

        *render_cmb.end_render_pass()
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

    auto deinit() {
        ImGui_ImplVulkan_Shutdown();
        ImGui::DestroyContext();
    }

    constexpr auto example_name() const noexcept -> std::string_view { return "Imgui"; }

  private:
    DeferInit<gpu::RenderPass>          m_render_pass;
    DeferInit<gpu::DescriptorPool>      m_descriptor_pool;
    std::vector<SubmissionResource>     m_submission_resources;
    std::vector<SwapchainImageResource> m_image_resources;
    usize                               m_current_frame = 0_usize;
};

auto main(std::span<const std::string_view> args) -> int {
    auto app = Application {};
    app.run(args);
    return 0;
}
