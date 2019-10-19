// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <unordered_map>
#include <variant>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

#ifdef STORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/core/Math.hpp>
#include <storm/render/resource/Fwd.hpp>

#include <storm/graphics/framegraph/Fwd.hpp>

namespace storm::graphics {
	struct FrameGraphTextureDesc {
		core::Extent extent;
		render::PixelFormat format = render::PixelFormat::Undefined;

		render::SampleCountFlag samples = render::SampleCountFlag::C1_BIT;
		std::uint32_t levels			= 1;
		std::uint32_t layers			= 1;

		render::ImageLayout layout =
			render::ImageLayout::Color_Attachment_Optimal;
	};

	struct FrameGraphBufferDesc {};

	class STORM_PUBLIC FrameGraphTextureResource {
	  public:
		explicit FrameGraphTextureResource(std::string &&name,
										   FrameGraphTextureDesc &&description,
										   render::DeviceCRef device);
		FrameGraphTextureResource(std::string &&name,
								  FrameGraphTextureDesc &&description,
								  render::Texture &texture,
								  render::DeviceCRef device);
		~FrameGraphTextureResource();

		FrameGraphTextureResource(const FrameGraphTextureResource &);
		FrameGraphTextureResource &operator=(const FrameGraphTextureResource &);

		FrameGraphTextureResource(FrameGraphTextureResource &&);
		FrameGraphTextureResource &operator=(FrameGraphTextureResource &&);

        [[nodiscard]] inline bool isTransient() const noexcept {
			return std::holds_alternative<render::TextureOwnedPtr>(m_texture);
		}
        [[nodiscard]] inline const FrameGraphTextureDesc &description() const
            noexcept {
			return m_description;
		}
        [[nodiscard]] inline render::Texture &texture() noexcept {
			if (std::holds_alternative<render::TextureRef>(m_texture))
				return std::get<render::TextureRef>(m_texture).get();

			auto &ptr = std::get<render::TextureOwnedPtr>(m_texture);

			STORM_EXPECTS(ptr != nullptr);

			return *ptr;
		}
        [[nodiscard]] inline const render::Texture &texture() const noexcept {
			if (std::holds_alternative<render::TextureRef>(m_texture))
				return std::get<render::TextureRef>(m_texture).get();

			auto &ptr = std::get<render::TextureOwnedPtr>(m_texture);

			STORM_EXPECTS(ptr != nullptr);

			return *ptr;
		}
        [[nodiscard]] inline std::string_view name() const noexcept {
			return m_name;
		}

	  private:
		using TextureVariant =
			std::variant<render::TextureOwnedPtr, render::TextureRef>;

		void realize();
		void derealize();

		render::DeviceCRef m_device;

		std::string m_name;
		FrameGraphTextureDesc m_description;
		TextureVariant m_texture;

		FramePassBaseObserverPtr m_creator;
		std::vector<FramePassBaseCRef> m_readers;
		std::vector<FramePassBaseCRef> m_writers;

		friend class FramePassBuilder;
		friend class FrameGraph;
		friend class FrameGraphBuilder;
	};

	class STORM_PUBLIC FrameGraphResources {
	  public:
		explicit FrameGraphResources(render::DeviceCRef device);
		~FrameGraphResources();

		FrameGraphResources(const FrameGraphResources &);
		FrameGraphResources &operator=(const FrameGraphResources &);

		FrameGraphResources(FrameGraphResources &&);
		FrameGraphResources &operator=(FrameGraphResources &&);

		FrameGraphTextureResource &
			setRetainedTexture(std::string &&name, FrameGraphTextureDesc &&desc,
							   render::Texture &texture);
		void setRetainedBuffer(std::string &&name, FrameGraphBufferDesc &&desc,
							   render::HardwareBuffer &buffer);

		FrameGraphTextureResource &
			addTransientTexture(std::string &&name,
								FrameGraphTextureDesc &&desc);

        [[nodiscard]] inline storm::core::span<
            const FrameGraphTextureResourceOwnedPtr>
			textures() const noexcept {
			return m_textures;
		}

	  private:
		render::DeviceCRef m_device;

		std::vector<FrameGraphTextureResourceOwnedPtr> m_textures;
	};
} // namespace storm::graphics
