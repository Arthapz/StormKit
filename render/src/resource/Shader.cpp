// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - STL - ///////////
#include <codecvt>
#include <fstream>
#include <streambuf>

/////////// - spirv-cross - ///////////
#include <spirv_cross_c.h>

/////////// - StormKit::log - ///////////
#include <storm/log/LogHandler.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>

#include <storm/render/resource/Shader.hpp>

using namespace storm;
using namespace storm::render;
using namespace storm::log;

/////////////////////////////////////
/////////////////////////////////////
Shader::Shader(std::filesystem::path filepath, ShaderStage type, const Device &device)
    : m_device { &device }, m_type { type }, m_descriptor_set_layout {
          m_device->createDescriptorSetLayout()
      } {
    s_spvc_counter++;

    auto stream     = std::ifstream { filepath.string(), std::ios::binary };
    const auto data = std::vector<char> { (std::istreambuf_iterator<char> { stream }),
                                          std::istreambuf_iterator<char> {} };
    std::transform(core::ranges::begin(data),
                   core::ranges::end(data),
                   std::back_inserter(m_source),
                   [](const auto b) { return static_cast<std::byte>(b); });
    compile();
    reflect();
}

/////////////////////////////////////
/////////////////////////////////////
Shader::Shader(core::span<const core::Byte> data, ShaderStage type, const Device &device)
    : m_device { &device }, m_type { type }, m_descriptor_set_layout {
          m_device->createDescriptorSetLayout()
      } {
    s_spvc_counter++;

    m_source.resize(std::size(data));
    std::copy(std::cbegin(data), std::cend(data), core::ranges::begin(m_source));
    compile();
    reflect();
}

/////////////////////////////////////
/////////////////////////////////////
Shader::Shader(core::span<const core::UInt32> data, ShaderStage type, const Device &device)
    : Shader { { reinterpret_cast<const core::Byte *>(std::data(data)),
                 std::size(data) * sizeof(core::UInt32) },
               type,
               device } {
}

/////////////////////////////////////
/////////////////////////////////////
Shader::~Shader() {
    if (s_spvc_counter == 0u && s_context != nullptr) spvc_context_destroy(s_context);
}

/////////////////////////////////////
/////////////////////////////////////
Shader::Shader(Shader &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Shader &Shader::operator=(Shader &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void Shader::compile() noexcept {
    const auto create_info =
        vk::ShaderModuleCreateInfo {}
            .setCodeSize(std::size(m_source))
            .setPCode(reinterpret_cast<const core::UInt32 *>(std::data(m_source)));

    m_vk_shader_module = m_device->createVkShaderModule(create_info);
}

/////////////////////////////////////
/////////////////////////////////////
void Shader::reflect() noexcept {
    using spvc_reflected_resources = const spvc_reflected_resource *;

    if (s_context == nullptr) { spvc_context_create(&s_context); }

    auto parsed = spvc_parsed_ir { nullptr };
    spvc_context_parse_spirv(s_context,
                             reinterpret_cast<SpvId *>(std::data(m_source)),
                             std::size(m_source) / sizeof(SpvId),
                             &parsed);

    auto compiler = spvc_compiler { nullptr };
    spvc_context_create_compiler(s_context,
                                 SPVC_BACKEND_NONE,
                                 parsed,
                                 SPVC_CAPTURE_MODE_COPY,
                                 &compiler);

    const auto add_bindings = [this, &compiler](spvc_reflected_resources &resources,
                                                std::size_t count,
                                                render::DescriptorType type) {
        for (auto i = 0u; i < count; ++i) {
            const auto set =
                spvc_compiler_get_decoration(compiler, resources[i].id, SpvDecorationDescriptorSet);
            const auto binding =
                spvc_compiler_get_decoration(compiler, resources[i].id, SpvDecorationBinding);
            const auto name = spvc_compiler_get_name(compiler, resources[i].id);

            m_descriptor_set_layout.addBinding({ binding,
                                                 type,
                                                 render::ShaderStage::Vertex |
                                                     render::ShaderStage::Fragment |
                                                     render::ShaderStage::Compute,
                                                 1 });
        }
    };

    auto resources = spvc_resources {};
    spvc_compiler_create_shader_resources(compiler, &resources);

    auto buffers      = spvc_reflected_resources { nullptr };
    auto buffer_count = std::size_t { 0u };
    spvc_resources_get_resource_list_for_type(resources,
                                              SPVC_RESOURCE_TYPE_UNIFORM_BUFFER,
                                              &buffers,
                                              &buffer_count);
    add_bindings(buffers, buffer_count, DescriptorType::Uniform_Buffer);

    auto sampled       = spvc_reflected_resources { nullptr };
    auto sampled_count = std::size_t { 0u };
    spvc_resources_get_resource_list_for_type(resources,
                                              SPVC_RESOURCE_TYPE_SAMPLED_IMAGE,
                                              &sampled,
                                              &sampled_count);
    add_bindings(sampled, sampled_count, DescriptorType::Sampled_Image);

    auto storage_images      = spvc_reflected_resources { nullptr };
    auto storage_image_count = std::size_t { 0u };
    spvc_resources_get_resource_list_for_type(resources,
                                              SPVC_RESOURCE_TYPE_STORAGE_IMAGE,
                                              &storage_images,
                                              &storage_image_count);
    add_bindings(sampled, sampled_count, DescriptorType::Storage_Image);

    auto storage_buffers      = spvc_reflected_resources { nullptr };
    auto storage_buffer_count = std::size_t { 0u };
    spvc_resources_get_resource_list_for_type(resources,
                                              SPVC_RESOURCE_TYPE_STORAGE_BUFFER,
                                              &storage_buffers,
                                              &storage_buffer_count);
    add_bindings(sampled, sampled_count, DescriptorType::Storage_Buffer);

    auto push_constants      = spvc_reflected_resources { nullptr };
    auto push_constant_count = std::size_t { 0u };
    spvc_resources_get_resource_list_for_type(resources,
                                              SPVC_RESOURCE_TYPE_PUSH_CONSTANT,
                                              &push_constants,
                                              &push_constant_count);

    m_descriptor_set_layout.bake();
}
