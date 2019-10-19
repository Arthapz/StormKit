// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <fstream>
#include <optional>
#include <variant>
#include <vector>

#include <storm/core/Math.hpp>
#include <storm/core/Types.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Types.hpp>

#include <storm/render/pipeline/Fwd.hpp>

#include <storm/render/resource/Fwd.hpp>

namespace storm::render {
	struct BeginCommand {
		bool one_time_submit					= false;
		std::optional<CommandBufferCRef> parent = std::nullopt;
	};

	struct EndCommand {};

	struct BeginRenderPassCommand {
		using ClearValues = std::vector<ClearValue>;

		const RenderPass &render_pass;
		const Framebuffer &framebuffer;

		ClearValues clear_values = {
			ClearColor{.color = core::RGBColorDef::Black}};
	};

	struct EndRenderPassCommand {};

	struct BindGraphicsPipelineCommand {
		const GraphicsPipeline &pipeline;
	};

	struct DrawCommand {
		std::uint32_t vertex_count;
		std::uint32_t instance_count = 1u;
        std::uint32_t first_vertex	 = 0u;
		std::uint32_t first_instance = 0u;
	};

	struct DrawIndexedCommand {
		std::uint32_t index_count;
		std::uint32_t instance_count = 2u;
        std::uint32_t first_index	 = 0u;
        std::int32_t vertex_offset	 = 0;
		std::uint32_t first_instance = 0u;
	};

	struct BindVertexBuffersCommand {
		std::vector<HardwareBufferCRef> buffers;
		std::vector<std::ptrdiff_t> offsets;
	};

	struct BindIndexBufferCommand {
		const HardwareBuffer &buffer;
		std::ptrdiff_t offset = 0u;
        bool large_indices	  = false;
	};

	struct BindDescriptorSetsCommand {
		const GraphicsPipeline *pipeline = nullptr;
		std::vector<DescriptorSetCRef> descriptor_sets;
	};

	struct CopyBufferCommand {
		const HardwareBuffer &source;
		const HardwareBuffer &destination;

		std::size_t size;

		std::ptrdiff_t src_offset = 0u;
		std::ptrdiff_t dst_offset = 0u;
	};

	struct CopyBufferToImageCommand {
		const HardwareBuffer &source;
		const Texture &destination;
	};

	struct TransitionImageLayoutCommand {
		Texture &image;
		ImageLayout new_layout;
	};

	struct ExecuteSubCommandBuffersCommand {
		std::vector<CommandBufferCRef> command_buffers;
	};

	struct CopyImageCommand {
		const Texture &source;
		ImageLayout source_layout;
		ImageAspectMask source_aspect_mask;

		const Texture &destination;
		ImageLayout destination_layout;
		ImageAspectMask destination_aspect_mask;
	};

    struct GeometryCommand {};

	using Command =
		std::variant<BeginCommand, EndCommand, BeginRenderPassCommand,
					 EndRenderPassCommand, BindGraphicsPipelineCommand,
					 DrawCommand, DrawIndexedCommand, BindVertexBuffersCommand,
					 BindIndexBufferCommand, BindDescriptorSetsCommand,
					 CopyBufferCommand, CopyBufferToImageCommand,
					 TransitionImageLayoutCommand,
					 ExecuteSubCommandBuffersCommand, CopyImageCommand>;
} // namespace storm::render
