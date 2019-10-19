// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#ifdef STORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

#include <functional>
#include <string>

#include <storm/core/NonCopyable.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/core/Math.hpp>
#include <storm/render/pipeline/Fwd.hpp>

#include <storm/graphics/framegraph/Fwd.hpp>

namespace storm::graphics {
	class STORM_PUBLIC FramePassBase : public core::NonCopyable {
	  public:
		explicit FramePassBase(std::string name);
		virtual ~FramePassBase() = 0;

		FramePassBase(FramePassBase &&);
		FramePassBase &operator=(FramePassBase &&);

        [[nodiscard]] inline std::string_view name() const noexcept {
			return m_name;
		}

		inline void setCullImune(bool imune) noexcept {
			m_cull_imune = imune;
		}

        [[nodiscard]] inline bool cullImune() const noexcept {
			return m_cull_imune;
		}

		inline void clear() {
			m_create_textures.clear();
			m_read_textures.clear();
			m_write_textures.clear();
		}

	  protected:
		virtual void build(FramePassBuilder &)				= 0;
		virtual void execute(render::RenderPass &, render::Framebuffer &,
							 render::CommandBuffer &) const = 0;

		std::string m_name;
		bool m_cull_imune = false;
		std::vector<FrameGraphTextureResourceRef> m_create_textures;
		std::vector<FrameGraphTextureResourceRef> m_read_textures;
		std::vector<FrameGraphTextureResourceRef> m_write_textures;

		friend class FrameGraph;
		friend class FrameGraphBuilder;
		friend class FramePassBuilder;
	};

	template <typename Data> class FramePass : public FramePassBase {
	  public:
		using PassData = Data;
		using BuildCallback =
			std::function<void(FramePassBuilder &, PassData &)>;
		using ExecuteCallback =
			std::function<void(const PassData &, render::RenderPass &,
							   render::Framebuffer &, render::CommandBuffer &)>;

		FramePass(std::string name, BuildCallback build_callback,
				  ExecuteCallback execute_callback);
		~FramePass() override;

		FramePass(FramePass &&);
		FramePass &operator=(FramePass &&);

		inline const PassData &data() const noexcept {
			return m_data;
		}

	  protected:
		void build(FramePassBuilder &) override;
		void execute(render::RenderPass &, render::Framebuffer &,
					 render::CommandBuffer &) const override;

	  private:
		BuildCallback m_build_callback;
		ExecuteCallback m_execute_callback;

		PassData m_data;
	};
} // namespace storm::graphics

#include "FramePass.inl"
