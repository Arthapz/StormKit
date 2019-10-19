// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/render/pipeline/GraphicsPipelineState.hpp>

#include <storm/graphics/core/Vertex.hpp>
#include <storm/graphics/scenegraph/Fwd.hpp>
#include <storm/graphics/scenegraph/ObjectTree.hpp>

#include <unordered_map>
#include <vector>

namespace storm::graphics {
	struct DrawableData {
		ObjectTreeNode::IndexType node;

		VertexArrayProxy vertex_array;
		IndexArrayProxy index_array;

		core::Matrix transform;
	};

	struct GPUMaterial {
		render::HardwareBufferOwnedPtr per_material_buffer;

		std::vector<render::DescriptorSet> descriptor_sets;
	};

	struct GPUDrawable {
		ObjectTreeNode::IndexType node;

		render::HardwareBufferOwnedPtr vertex_buffer;
		render::HardwareBufferOwnedPtr index_buffer;
		bool use_large_indices = false;
		render::HardwareBufferOwnedPtr per_drawable_buffer;

		std::vector<render::DescriptorSet> descriptor_sets;
	};

	using TransformData =
		core::NamedType<core::Matrix, struct TransformDataTag>;

	using UpdateData = std::variant<TransformData>;
	class STORM_PUBLIC DrawableManager : public core::NonCopyable {
	  public:
		using Drawables			   = std::vector<GPUDrawable>;
		using DrawablesPerMaterial = std::unordered_map<Material, Drawables>;
		using MaterialsPerPipeline =
			std::unordered_map<render::GraphicsPipelineState,
							   DrawablesPerMaterial>;

		DrawableManager(const render::Device &device,
						std::uint32_t image_count);
		~DrawableManager();

		DrawableManager(DrawableManager &&);
		DrawableManager &operator=(DrawableManager &&);

		void addDrawable(const render::GraphicsPipelineState &state,
						 const Material &material, DrawableData drawable);
		void updateDrawable(const render::GraphicsPipelineState &state,
							const Material &material,
							ObjectTreeNode::IndexType index, UpdateData data);
		void removeDrawable(const render::GraphicsPipelineState &state,
							const Material &material,
							ObjectTreeNode::IndexType index);
		inline bool hasDrawable(const render::GraphicsPipelineState &state,
								const Material &material,
								ObjectTreeNode::IndexType index) {
			return getDrawable(state, material, index) !=
				   std::cend(m_drawables.at(state).at(material));
		}
		inline bool hasMaterial(const Material &material) {
			return std::size(m_gpu_materials.at(material).descriptor_sets);
		}

		inline const MaterialsPerPipeline &drawables() const noexcept {
			return m_drawables;
		}

	  private:
		void createMaterial(const Material &material);

		render::DeviceCRef m_device;
		std::uint32_t m_image_count;

		Drawables::iterator
			getDrawable(const render::GraphicsPipelineState &state,
						const Material &material,
						ObjectTreeNode::IndexType type);

		MaterialsPerPipeline m_drawables;
		std::unordered_map<Material, GPUMaterial> m_gpu_materials;
		// std::unordered_map<render::GraphicsPipelineLayout,
	};
} // namespace storm::graphics
