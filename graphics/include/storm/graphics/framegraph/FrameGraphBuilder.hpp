// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#ifdef STORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>

#include <storm/core/NonCopyable.hpp>

#include <storm/render/core/Fwd.hpp>
#include <storm/render/pipeline/Fwd.hpp>
#include <storm/render/resource/Fwd.hpp>

#include <storm/graphics/framegraph/FrameGraphResources.hpp>
#include <storm/graphics/framegraph/FramePassBuilder.hpp>
#include <storm/graphics/framegraph/Fwd.hpp>

namespace storm::graphics {
	class STORM_PUBLIC FrameGraphBuilder : public core::NonCopyable {
	  public:
		explicit FrameGraphBuilder(const render::Device &device);
		~FrameGraphBuilder();

		FrameGraphBuilder(FrameGraphBuilder &&);
		FrameGraphBuilder &operator=(FrameGraphBuilder &&);

		template <typename Data>
		FramePass<Data> &
			addPass(std::string name,
					typename FramePass<Data>::BuildCallback build_callback,
					typename FramePass<Data>::ExecuteCallback execute_callback);

		inline void setBackbufferName(std::string backbuffer_name) {
			m_backbuffer_name = std::move(backbuffer_name);
		}
		inline FrameGraphTextureResource &
			setRetainedTexture(std::string name, FrameGraphTextureDesc desc,
							   render::Texture &texture);
		inline void setRetainedBuffer(std::string name,
									  FrameGraphBufferDesc desc,
									  render::HardwareBuffer &buffer);

        [[nodiscard]] FrameGraph compile();

	  private:
		using FrameGraphPasses = std::vector<FramePassBaseOwnedPtr>;

		std::string m_backbuffer_name;

		FrameGraphResources m_resources;
		FrameGraphPasses m_passes;

		render::DeviceCRef m_device;
	};
} // namespace storm::graphics

#include "FrameGraphBuilder.inl"
