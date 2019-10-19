// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

#include <storm/core/Filesystem.hpp>
#include <storm/core/NonCopyable.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>

#include <storm/window/Window.hpp>

extern "C" {
	struct TBuiltInResource;
}

namespace storm::render {
	class STORM_PUBLIC Shader : public core::NonCopyable {
	  public:
		enum class Language { SPIRV, GLSL };

		Shader(core::filesystem::path filepath, Language language,
			   ShaderType type, const Device &device);
		Shader(core::span<const std::byte> data, Language language,
			   ShaderType type, const Device &device);
		~Shader();

		Shader(Shader &&);
		Shader &operator=(Shader &&);

		inline ShaderType type() const noexcept {
			return m_type;
		}
		inline storm::core::span<const std::byte> source() const noexcept {
			return m_source;
		}

		inline const Device &device() const noexcept {
			return *m_device;
		}

		inline VkShaderModule vkShaderModule() const noexcept {
			STORM_EXPECTS(m_vk_shader_module != VK_NULL_HANDLE);
			return m_vk_shader_module;
		}

		inline operator VkShaderModule() const noexcept {
			STORM_EXPECTS(m_vk_shader_module != VK_NULL_HANDLE);
			return m_vk_shader_module;
		}
	  private:
		void compile() noexcept;
		void transpileToSpirv(Language language,
							  gsl::span<const std::byte> source);
		void initResources(TBuiltInResource &resources);

		DeviceConstObserverPtr m_device;

		ShaderType m_type;
		std::vector<std::byte> m_source;

		VkShaderModule m_vk_shader_module = VK_NULL_HANDLE;
	};

} // namespace storm::render
