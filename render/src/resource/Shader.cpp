// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <codecvt>
#include <fstream>
#include <streambuf>

#include <storm/log/LogHandler.hpp>

#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>

#include <storm/render/resource/Shader.hpp>

using namespace storm;
using namespace storm::render;
using namespace storm::log;

/////////////////////////////////////
/////////////////////////////////////
Shader::Shader(std::filesystem::path filepath, ShaderStage type, const Device &device)
    : m_device { &device }, m_type { type } {
    auto stream     = std::ifstream { filepath.string(), std::ios::binary };
    const auto data = std::vector<char> { (std::istreambuf_iterator<char> { stream }),
                                          std::istreambuf_iterator<char> {} };
    std::transform(core::ranges::begin(data),
                   core::ranges::end(data),
                   std::back_inserter(m_source),
                   [](const auto b) { return static_cast<std::byte>(b); });
    compile();
}

/////////////////////////////////////
/////////////////////////////////////
Shader::Shader(core::span<const core::Byte> data, ShaderStage type, const Device &device)
    : m_device { &device }, m_type { type } {
    m_source.resize(std::size(data));
    std::copy(std::cbegin(data), std::cend(data), core::ranges::begin(m_source));
    compile();
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
Shader::~Shader() = default;

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
