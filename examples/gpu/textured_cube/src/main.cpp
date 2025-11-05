// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <cstddef> // offsetof

import std;
import stormkit;

import gpu_app;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/log/log_macro.hpp>
#include <stormkit/main/main_macro.hpp>

LOGGER("stormkit.examples.gpu.textured_cube");

#ifndef SHADER_DIR
    #define SHADER_DIR "../share/shaders"
#endif

#ifndef TEXTURE_DIR
    #define TEXTURE_DIR "../share/textures"
#endif

namespace stdc = std::chrono;
namespace stdr = std::ranges;

using clock = stdc::high_resolution_clock;

using namespace std::literals;
using namespace stormkit;
using namespace stormkit::literals;

struct SubmissionResource {
    gpu::Fence         in_flight;
    gpu::Semaphore     image_available;
    gpu::CommandBuffer render_cmb;
    gpu::Buffer        viewer_buffer;
    gpu::DescriptorSet descriptor_set;
};

struct SwapchainImageResource {
    Ref<const gpu::Image> image;
    gpu::ImageView        view;
    gpu::Image            depth_image;
    gpu::ImageView        depth_view;
    gpu::FrameBuffer      framebuffer;
    gpu::Semaphore        render_finished;
};

struct Vertex {
    math::vec3f position;
    math::vec2f uv;

    static constexpr auto attribute_descriptions() noexcept
      -> std::array<gpu::VertexInputAttributeDescription, 2> {
        return {
            gpu::VertexInputAttributeDescription {
                                                  0, 0,
                                                  gpu::PixelFormat::RGB32F,
                                                  offsetof(Vertex, position) },
            gpu::VertexInputAttributeDescription {
                                                  1, 0,
                                                  gpu::PixelFormat::RG32F,
                                                  offsetof(Vertex, uv)       },
        };
    }

    static constexpr auto binding_description() noexcept -> gpu::VertexBindingDescription {
        return { 0, sizeof(Vertex), gpu::VertexInputRate::VERTEX };
    }
};

static constexpr auto VERTICES = std::array<Vertex, 36> {
    Vertex { { -1.f, -1.f, -1.f }, { 2.f / 3.f, 3.f / 4.f } }, // -X side
    { { -1.f, -1.f, 1.f },  { 1.f / 3.f, 3.f / 4.f } },
    { { -1.f, 1.f, 1.f },   { 1.f / 3.f, 1.f }       },
    { { -1.f, 1.f, 1.f },   { 1.f / 3.f, 1.f }       },
    { { -1.f, 1.f, -1.f },  { 2.f / 3.f, 1.f }       },
    { { -1.f, -1.f, -1.f }, { 2.f / 3.f, 3.f / 4.f } },

    { { -1.f, -1.f, -1.f }, { 1.f / 3.f, 2.f / 4.f } }, // -Z side
    { { 1.f, 1.f, -1.f },   { 0.f, 1.f / 4.f }       },
    { { 1.f, -1.f, -1.f },  { 0.f, 2.f / 4.f }       },
    { { -1.f, -1.f, -1.f }, { 1.f / 3.f, 2.f / 4.f } },
    { { -1.f, 1.f, -1.f },  { 1.f / 3.f, 1.f / 4.f } },
    { { 1.f, 1.f, -1.f },   { 0.f, 1.f / 4.f }       },

    { { -1.f, -1.f, -1.f }, { 2.f / 3.f, 2.f / 4.f } }, // -Y side
    { { 1.f, -1.f, -1.f },  { 2.f / 3.f, 3.f / 4.f } },
    { { 1.f, -1.f, 1.f },   { 1.f / 3.f, 3.f / 4.f } },
    { { -1.f, -1.f, -1.f }, { 2.f / 3.f, 2.f / 4.f } },
    { { 1.f, -1.f, 1.f },   { 1.f / 3.f, 3.f / 4.f } },
    { { -1.f, -1.f, 1.f },  { 1.f / 3.f, 2.f / 4.f } },

    { { -1.f, 1.f, -1.f },  { 2.f / 3.f, 0.f }       }, // +Y side
    { { -1.f, 1.f, 1.f },   { 1.f / 3.f, 0.f }       },
    { { 1.f, 1.f, 1.f },    { 1.f / 3.f, 1.f / 4.f } },
    { { -1.f, 1.f, -1.f },  { 2.f / 3.f, 0.f }       },
    { { 1.f, 1.f, 1.f },    { 1.f / 3.f, 1.f / 4.f } },
    { { 1.f, 1.f, -1.f },   { 2.f / 3.f, 1.f / 4.f } },

    { { 1.f, 1.f, -1.f },   { 2.f / 3.f, 1.f / 4.f } }, // +X side
    { { 1.f, 1.f, 1.f },    { 1.f / 3.f, 1.f / 4.f } },
    { { 1.f, -1.f, 1.f },   { 1.f / 3.f, 2.f / 4.f } },
    { { 1.f, -1.f, 1.f },   { 1.f / 3.f, 2.f / 4.f } },
    { { 1.f, -1.f, -1.f },  { 2.f / 3.f, 2.f / 4.f } },
    { { 1.f, 1.f, -1.f },   { 2.f / 3.f, 1.f / 4.f } },

    { { -1.f, 1.f, 1.f },   { 2.f / 3.f, 1.f / 4.f } }, // +Z side
    { { -1.f, -1.f, 1.f },  { 2.f / 3.f, 2.f / 4.f } },
    { { 1.f, 1.f, 1.f },    { 1.f, 1.f / 4.f }       },
    { { -1.f, -1.f, 1.f },  { 2.f / 3.f, 2.f / 4.f } },
    { { 1.f, -1.f, 1.f },   { 1.f, 2.f / 4.f }       },
    { { 1.f, 1.f, 1.f },    { 1.f, 1.f / 4.f }       },
};

struct ViewerData {
    math::mat4f proj;
    math::mat4f view;
    math::mat4f model;

    static constexpr auto layout_binding() -> gpu::DescriptorSetLayoutBinding {
        return { 0, gpu::DescriptorType::UNIFORM_BUFFER, gpu::ShaderStageFlag::VERTEX, 1 };
    }
};

static constexpr auto VERTICES_SIZE = sizeof(Vertex) * stdr::size(VERTICES);

static constexpr auto BUFFERING_COUNT = 2;

class Application: public base::Application {
  public:
    auto init_example() {
        static constexpr auto POOL_SIZES = std::array {
            gpu::DescriptorPool::Size {
                                       .type             = gpu::DescriptorType::UNIFORM_BUFFER,
                                       .descriptor_count = BUFFERING_COUNT,
                                       },
            gpu::DescriptorPool::Size { .type = gpu::DescriptorType::COMBINED_IMAGE_SAMPLER,
                                       .descriptor_count = BUFFERING_COUNT }
        };
        m_descriptor_pool = gpu::DescriptorPool::create(m_device, POOL_SIZES, BUFFERING_COUNT * 2)
                              .transform_error(monadic::assert("Failed to create descriptor pool"))
                              .value();

        // load shaders
        m_vertex_shader = gpu::Shader::load_from_file(m_device,
                                                      SHADER_DIR "/textured_cube.spv",
                                                      gpu::ShaderStageFlag::VERTEX)
                            .transform_error(monadic::assert("Failed to load vertex shader"))
                            .value();

        m_fragment_shader = gpu::Shader::load_from_file(m_device,
                                                        SHADER_DIR "/textured_cube.spv",
                                                        gpu::ShaderStageFlag::FRAGMENT)
                              .transform_error(monadic::assert("Failed to load fragment shader"))
                              .value();

        m_descriptor_set_layout
          = gpu::DescriptorSetLayout::
              create(m_device,
                     into_dyn_array(ViewerData::layout_binding(),
                                    gpu::DescriptorSetLayoutBinding {
                                      1,
                                      gpu::DescriptorType::COMBINED_IMAGE_SAMPLER,
                                      gpu::ShaderStageFlag::FRAGMENT,
                                      1 }))
                .transform_error(monadic::assert("Failed to create descriptor set layout"))
                .value();
        m_pipeline_layout = gpu::PipelineLayout::create(
                              m_device,
                              { .descriptor_set_layouts = to_refs(*m_descriptor_set_layout) })
                              .transform_error(monadic::assert("Failed to create pipeline layout"))
                              .value();
        // initialize render pass
        const auto depth_format = [this] {
            const auto formats_properties = m_physical_device->formats_properties();
            const auto candidates = std::array { gpu::PixelFormat::DEPTH32F,
                                                 gpu::PixelFormat::DEPTH32F_STENCIL8U,
                                                 gpu::PixelFormat::DEPTH24_UNORM_STENCIL8U };

            for (const auto format : candidates) {
                const auto properties = stdr::find_if(formats_properties,
                                                      [format](const auto& pair) {
                                                          return pair.first == format;
                                                      });
                ENSURES(properties != stdr::cend(formats_properties));
                if (check_flag_bit(properties->second.optimal_tiling_features,
                                   gpu::FormatFeatureFlag::DEPTH_STENCIL_ATTACHMENT)) {
                    return format;
                }
            }

            ensures(false, "No supported depth format found !");
            std::unreachable();
        }();

        const auto depth_aspect_flag = [depth_format] {
            auto flag = gpu::ImageAspectFlag::DEPTH;
            if (gpu::is_depth_stencil_format(depth_format)) flag |= gpu::ImageAspectFlag::STENCIL;
            return flag;
        }();

        m_render_pass
      = gpu::RenderPass::
          create(m_device,
                 { .attachments = { {
                     .format = m_swapchain->pixel_format(),
                   }, {
                       .format = depth_format,
                       .destination_layout = gpu::ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                   }},
                   .subpasses   = { gpu::Subpass{ .bind_point            = gpu::PipelineBindPoint::GRAPHICS,
                                      .color_attachment_refs = { { .attachment_id = 0u }, }, .depth_attachment_ref = gpu::Subpass::Ref{.attachment_id = 1u, .layout = gpu::ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL, }, }, }, })
            .transform_error(monadic::assert("Failed to create render pass"))
            .value();

        // initialize render pipeline
        const auto window_extent   = m_window->extent();
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
        .rasterization_state = {
            .cull_mode = gpu::CullModeFlag::BACK,
            .front_face = gpu::FrontFace::CLOCKWISE,
        },
        .color_blend_state
        = { .attachments = { { .blend_enable           = true,
                               .src_color_blend_factor = gpu::BlendFactor::SRC_ALPHA,
                               .dst_color_blend_factor = gpu::BlendFactor::ONE_MINUS_SRC_ALPHA,
                               .src_alpha_blend_factor = gpu::BlendFactor::SRC_ALPHA,
                               .dst_alpha_blend_factor = gpu::BlendFactor::ONE_MINUS_SRC_ALPHA,
                               .alpha_blend_operation  = gpu::BlendOperation::ADD, }, }, },
        .shader_state  = to_refs(*m_vertex_shader, *m_fragment_shader),
        .vertex_input_state = {
            .binding_descriptions = into_dyn_array(Vertex::binding_description()),
            .input_attribute_descriptions = to_dyn_array(Vertex::attribute_descriptions()),
        },
            .depth_stencil_state = {
            .depth_test_enable =  true,
            .depth_write_enable = true
        },
    };

        m_pipeline = gpu::Pipeline::create(m_device, state, m_pipeline_layout, m_render_pass)
                       .transform_error(monadic::assert("Failed to create raster pipeline"))
                       .value();

        // load texture
        auto image = image::Image {};
        image.load_from_file(TEXTURE_DIR "/cube.png").transform_error(monadic::assert()).value();

        m_texture = gpu::Image::create(m_device,
                                       { .extent = image.extent(),
                                         .format = gpu::PixelFormat::RGBA8_UNORM,
                                         .usages = gpu::ImageUsageFlag::SAMPLED
                                                   | gpu::ImageUsageFlag::TRANSFER_DST,
                                         .property = gpu::MemoryPropertyFlag::DEVICE_LOCAL })
                      .transform_error(monadic::assert("Failed to allocate texture"))
                      .value();

        {
            auto staging_buffer
              = gpu::Buffer::create(m_device,
                                    { .usages = gpu::BufferUsageFlag::TRANSFER_SRC,
                                      .size   = image.size() })
                  .transform_error(monadic::assert("Failed to allocate gpu texture staging buffer"))
                  .value();

            staging_buffer.upload(image.data())
              .transform_error(monadic::assert("Failed to upload texture data to staging buffer"))
              .value();

            auto
              cpy_fence = gpu::Fence::create(m_device)
                            .transform_error(monadic::
                                               assert("Failed to create copy texture buffer fence"))
                            .value();

            const auto copy = {
                gpu::BufferImageCopy {
                                      .buffer_offset       = 0,
                                      .buffer_row_length   = 0,
                                      .buffer_image_height = 0,
                                      .subresource_layers  = {},
                                      .offset              = {},
                                      .extent              = image.extent() }
            };
            auto copy_cmb = m_command_pool->create_command_buffer()
                              .transform_error(monadic::
                                                 assert("Failed to allocate copy texture buffer "
                                                        "commandbuffer"))
                              .value();

            copy_cmb.begin()
              .transform_error(monadic::assert("Failed to begin texture upload command buffer"))
              .value()
              ->begin_debug_region("Upload texture data")
              .transition_image_layout(m_texture,
                                       gpu::ImageLayout::UNDEFINED,
                                       gpu::ImageLayout::TRANSFER_DST_OPTIMAL)
              .copy_buffer_to_image(staging_buffer, m_texture, as_view(copy))
              .transition_image_layout(m_texture,
                                       gpu::ImageLayout::TRANSFER_DST_OPTIMAL,
                                       gpu::ImageLayout::SHADER_READ_ONLY_OPTIMAL)
              .end_debug_region()
              .end()
              .transform_error(monadic::assert("Failed to end texture upload command buffer"))
              .value()
              ->submit(m_raster_queue, {}, {}, {}, as_ref(cpy_fence))
              .transform_error(monadic::assert("Failed to submit texture upload command buffer"))
              .value();

            *cpy_fence.wait().transform_error(monadic::assert());
        }

        m_texture_view = gpu::ImageView::create(m_device, m_texture)
                           .transform_error(monadic::assert("Failed to create texture view"))
                           .value();

        m_sampler = gpu::Sampler::create(m_device, {})
                      .transform_error(monadic::assert("Failed to create sampler"))
                      .value();

        // create present engine resources
        m_submission_resources = std::vector<SubmissionResource> {};
        m_submission_resources.reserve(BUFFERING_COUNT);

        for (auto _ : range(BUFFERING_COUNT)) {
            m_submission_resources.push_back({
              .in_flight = gpu::Fence::create_signaled(m_device)
                             .transform_error(core::monadic::
                                                assert("Failed to create swapchain image "
                                                       "in flight fence"))
                             .value(),
              .image_available = gpu::Semaphore::create(m_device)
                                   .transform_error(core::monadic::
                                                      assert("Failed to create present "
                                                             "wait semaphore"))
                                   .value(),
              .render_cmb = m_command_pool->create_command_buffer()
                              .transform_error(monadic::assert("Failed to create "
                                                               "transition command "
                                                               "buffers"))
                              .value(),
              .viewer_buffer = gpu::Buffer::create(m_device,
                                                   {
                                                     .usages = gpu::BufferUsageFlag::UNIFORM,
                                                     .size   = sizeof(ViewerData),
                                                   },
                                                   true)
                                 .transform_error(monadic::
                                                    assert("Failed to allocate gpu viewer buffer"))
                                 .value(),
              .descriptor_set = m_descriptor_pool->create_descriptor_set(m_descriptor_set_layout)
                                  .transform_error(monadic::
                                                     assert("Failed to create descriptor set"))
                                  .value(),
            });
            auto&      res  = m_submission_resources.back();
            const auto sets = std::array<gpu::Descriptor, 2> {
                gpu::BufferDescriptor {
                                       .binding = 0,
                                       .buffer  = as_ref(res.viewer_buffer),
                                       .range   = sizeof(ViewerData),
                                       .offset  = 0,
                                       },
                gpu::ImageDescriptor {
                                       .binding    = 1,
                                       .layout     = gpu::ImageLayout::SHADER_READ_ONLY_OPTIMAL,
                                       .image_view = as_ref(*m_texture_view),
                                       .sampler    = as_ref(*m_sampler),
                                       }
            };
            res.descriptor_set.update(sets);
        }

        const auto& images = m_swapchain->images();

        const auto image_count = stdr::size(images);
        auto       transition_cmbs
          = m_command_pool->create_command_buffers(image_count)
              .transform_error(monadic::assert("Failed to create transition command buffers"))
              .value();

        m_image_resources = std::vector<SwapchainImageResource> {};
        m_image_resources.reserve(stdr::size(images));

        auto image_index = 0u;
        for (const auto& swap_image : images) {
            auto view = gpu::ImageView::create(m_device, swap_image)
                          .transform_error(core::monadic::
                                             assert("Failed to create swapchain image view"))
                          .value();

            auto depth_image
              = gpu::Image::create(m_device,
                                   gpu::Image::CreateInfo {
                                     .extent   = swap_image.extent(),
                                     .format   = depth_format,
                                     .usages   = gpu::ImageUsageFlag::DEPTH_STENCIL_ATTACHMENT,
                                     .property = gpu::MemoryPropertyFlag::DEVICE_LOCAL })
                  .transform_error(core::monadic::assert("Failed to create depth image"))
                  .value();

            auto depth_view = gpu::ImageView::create(m_device,
                                                     depth_image,
                                                     gpu::ImageViewType::T2D,
                                                     { .aspect_mask = depth_aspect_flag })
                                .transform_error(core::monadic::
                                                   assert("Failed to create depth image view"))
                                .value();

            auto framebuffer = m_render_pass
                                 ->create_frame_buffer(m_device,
                                                       window_extent,
                                                       to_refs(view, depth_view))
                                 .transform_error(core::monadic::assert(
                                   std::format("Failed to create framebuffer for image {}",
                                               image_index)))
                                 .value();

            m_image_resources.push_back({
              .image           = as_ref(swap_image),
              .view            = std::move(view),
              .depth_image     = std::move(depth_image),
              .depth_view      = std::move(depth_view),
              .framebuffer     = std::move(framebuffer),
              .render_finished = gpu::Semaphore::create(m_device)
                                   .transform_error(core::monadic::assert("Failed to create render "
                                                                          "signal semaphore"))
                                   .value(),
            });

            const auto& resources = m_image_resources.back();

            auto& transition_cmb = transition_cmbs[image_index];
            *transition_cmb.begin(true)
               .transform_error(monadic::
                                  assert("Failed to begin texture transition command buffer"))
               .value()
               ->begin_debug_region(std::format("transition image {}", image_index))
               .transition_image_layout(swap_image,
                                        gpu::ImageLayout::UNDEFINED,
                                        gpu::ImageLayout::PRESENT_SRC)
               .transition_image_layout(resources.depth_image,
                                        gpu::ImageLayout::UNDEFINED,
                                        gpu::ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                        { .aspect_mask = depth_aspect_flag })
               .end_debug_region()
               .end()
               .transform_error(monadic::
                                  assert("Failed to begin texture transition command buffer"))
               .transform(monadic::discard());

            ++image_index;
        }

        const auto fence = gpu::Fence::create(m_device)
                             .transform_error(monadic::assert("Failed to create transition fence"))
                             .value();

        const auto cmbs = to_refs(transition_cmbs);
        m_raster_queue->submit({ .command_buffers = cmbs }, as_ref(fence))
          .transform_error(monadic::assert("Failed to submit texture transition command buffers"))
          .value();

        // setup vertex buffer
        m_vertex_buffer = gpu::Buffer::create(m_device,
                                              { .usages = gpu::BufferUsageFlag::VERTEX
                                                          | gpu::BufferUsageFlag::TRANSFER_DST,
                                                .size     = VERTICES_SIZE,
                                                .property = gpu::MemoryPropertyFlag::DEVICE_LOCAL })
                            .transform_error(monadic::
                                               assert("Failed to allocate gpu vertex buffer"))
                            .value();

        {
            auto staging_buffer
              = gpu::Buffer::create(m_device,
                                    { .usages = gpu::BufferUsageFlag::TRANSFER_SRC,
                                      .size   = VERTICES_SIZE })
                  .transform_error(monadic::assert("Failed to allocate gpu vertex staging buffer"))
                  .value();

            staging_buffer.upload(VERTICES)
              .transform_error(monadic::assert("Failed to upload vertex data to staging buffer"))
              .value();

            auto
              cpy_fence = gpu::Fence::create(m_device)
                            .transform_error(monadic::
                                               assert("Failed to create copy vertex buffer fence"))
                            .value();

            auto
              copy_cmb = m_command_pool->create_command_buffer()
                           .transform_error(monadic::assert("Failed to allocate copy vertex buffer "
                                                            "commandbuffer"))
                           .value();

            copy_cmb.begin()
              .transform_error(monadic::assert("Failed to begin vertices upload command buffer"))
              .value()
              ->begin_debug_region("Upload vertex data to vertex buffer")
              .copy_buffer(staging_buffer, m_vertex_buffer, VERTICES_SIZE)
              .end_debug_region()
              .end()
              .transform_error(monadic::assert("Failed to begin vertices upload command buffer"))
              .value()
              ->submit(m_raster_queue, {}, {}, {}, as_ref(cpy_fence));

            cpy_fence.wait().transform_error(monadic::assert());
        }

        // wait for transition to be done
        fence.wait().transform_error(monadic::assert());
    }

    auto run_example() {
        using SecondF                = stdc::duration<float, stdc::seconds::period>;
        const auto window_extent     = m_window->extent();
        const auto window_extent_f32 = window_extent.to<f32>();
        auto       viewer_data       = ViewerData {
            .proj  = math::perspective(math::radians(45.f),
                                       window_extent_f32.width / window_extent_f32.height,
                                       0.1f,
                                       100.f),
            .view  = math::look_at(math::vec3f { 0.f, 3.f, 5.f },
                                   { 0.f, 0.f, 0.f },
                                   { 0.f, 1.f, 0.f }),
            .model = math::mat4f::identity(),
        };
        LOG_MODULE.flush();

        const auto current_time = clock::now();

        // get next swapchain image
        auto& submission_resource = m_submission_resources[m_current_frame];

        const auto& wait      = submission_resource.image_available;
        auto&       in_flight = submission_resource.in_flight;

        const auto acquire_next_image = bind_front(&gpu::SwapChain::acquire_next_image,
                                                   &*m_swapchain,
                                                   100ms,
                                                   std::cref(wait));
        const auto extract_index      = [](auto&& _result) static noexcept {
            auto&& [result, _image_index] = _result;
            return _image_index;
        };

        const auto
          image_index = in_flight.wait()
                          .transform([&in_flight](auto&&) noexcept { in_flight.reset(); })
                          .and_then(acquire_next_image)
                          .transform(extract_index)
                          .transform_error(monadic::
                                             assert("Failed to acquire next swapchain image"))
                          .value();

        const auto& swapchain_image_resource = m_image_resources[image_index];
        const auto& framebuffer              = swapchain_image_resource.framebuffer;
        const auto& signal                   = swapchain_image_resource.render_finished;

        // update viewer data and upload
        const auto time   = stdc::duration_cast<SecondF>(current_time - m_start_time).count();
        viewer_data.model = math::rotate(math::mat4f::identity(),
                                         time * math::radians(90.f),
                                         math::vec3f { 0.f, 1.f, 0.f });

        auto& viewer_buffer = submission_resource.viewer_buffer;
        viewer_buffer.upload(viewer_data);

        // render in it
        auto&       render_cmb     = submission_resource.render_cmb;
        const auto& descriptor_set = submission_resource.descriptor_set;

        static constexpr auto CLEAR_VALUES = std::array<gpu::ClearValue, 2> {
            gpu::ClearColor { .color = RGBColorDef::SILVER<float> },
            gpu::ClearDepthStencil {}
        };
        static constexpr auto OFFSETS        = std::array { 0_u64 };
        static constexpr auto PIPELINE_FLAGS = std::array {
            gpu::PipelineStageFlag::COLOR_ATTACHMENT_OUTPUT
        };

        render_cmb.reset()
          .transform_error(monadic::assert("Failed to reset render command buffer"))
          .value()
          ->begin()
          .transform_error(monadic::assert("Failed to begin render command buffer"))
          .value()
          ->begin_debug_region("Render textured cube")
          .begin_render_pass(m_render_pass, framebuffer, CLEAR_VALUES)
          .bind_pipeline(m_pipeline)
          .bind_vertex_buffers(to_refs(*m_vertex_buffer), OFFSETS)
          .bind_descriptor_sets(m_pipeline, m_pipeline_layout, as_refs(descriptor_set), {})
          .draw(stdr::size(VERTICES))
          .end_render_pass()
          .end_debug_region()
          .end()
          .transform_error(monadic::assert("Failed to end render command buffer"))
          .value()
          ->submit(m_raster_queue,
                   as_refs(wait),
                   PIPELINE_FLAGS,
                   as_refs(signal),
                   as_ref(in_flight))
          .transform_error(monadic::assert("Failed to submit render command buffer"))
          .value();

        // present it
        auto update_current_frame = [this](auto&&) mutable noexcept {
            if (++m_current_frame >= BUFFERING_COUNT) m_current_frame = 0;
        };

        m_raster_queue->present(as_refs(*m_swapchain), as_refs(signal), as_view(image_index))
          .transform(update_current_frame)
          .transform_error(monadic::assert("Failed to present swapchain image"));

        m_raster_queue->wait_idle();
        m_device->wait_idle();
    }

    constexpr auto example_name() const noexcept -> std::string_view { return "Textured Cube"; }

  private:
    DeferInit<gpu::DescriptorPool> m_descriptor_pool;

    DeferInit<gpu::Shader>              m_vertex_shader;
    DeferInit<gpu::Shader>              m_fragment_shader;
    DeferInit<gpu::DescriptorSetLayout> m_descriptor_set_layout;
    DeferInit<gpu::PipelineLayout>      m_pipeline_layout;
    DeferInit<gpu::RenderPass>          m_render_pass;
    DeferInit<gpu::Pipeline>            m_pipeline;
    DeferInit<gpu::Image>               m_texture;
    DeferInit<gpu::ImageView>           m_texture_view;
    DeferInit<gpu::Sampler>             m_sampler;
    std::vector<SubmissionResource>     m_submission_resources;
    std::vector<SwapchainImageResource> m_image_resources;
    DeferInit<gpu::Buffer>              m_vertex_buffer;
    usize                               m_current_frame = 0_usize;
    decltype(clock::now())              m_start_time    = clock::now();
};

auto main(std::span<const std::string_view> args) -> int {
    auto app = Application {};
    app.run(args);
    return 0;
}
