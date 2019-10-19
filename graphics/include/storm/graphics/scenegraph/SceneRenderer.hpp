// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <unordered_map>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Tree.hpp>

#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/graphics/framegraph/Fwd.hpp>
#include <storm/graphics/scenegraph/Fwd.hpp>
#include <storm/graphics/scenegraph/ObjectTree.hpp>

namespace storm::graphics {
	struct SceneVisitorContext;
	class STORM_PUBLIC SceneRenderer : public core::NonCopyable {
	  public:
		explicit SceneRenderer(render::Surface &surface,
							   render::Device &device);
		~SceneRenderer();

		SceneRenderer(SceneRenderer &&);
		SceneRenderer &operator=(SceneRenderer &&);

		inline FrameGraph &framegraph(std::uint32_t index) {
			STORM_EXPECTS(index < std::size(m_frame_graphs));

			return *m_frame_graphs[index];
		}

		inline const DrawableManager &
			drawableManagerFor(const SceneGraph &graph) const {
			return *m_contexts.at(&graph).drawables;
		}

		std::uint32_t prepareRender(SceneGraph &graph);
		void render(const SceneGraph &graph);

		void genDotFiles(const SceneGraph &graph);

	  private:
		struct SceneGraphContext {
			ObjectTreeOwnedPtr tree;
			SceneGraphObserverOwnedPtr observer;
			DrawableManagerOwnedPtr drawables;

			render::HardwareBufferOwnedPtr per_frame_buffer;
			render::HardwareBufferOwnedPtr per_frame_staging_buffer;
			ObjectTreeNode::DirtyBitType per_frame_dirty = 0;

			render::CommandBufferOwnedPtr transfert_cmb;
			render::SemaphoreOwnedPtr transfert_semaphore;
		};

		using ContextMap =
			std::unordered_map<const SceneGraph *, SceneGraphContext>;

		ContextMap::iterator addNewContext(SceneGraph &graph);
		void traverse(SceneGraphContext &context);
		void traverseSubTree(SceneGraphContext &context,
							 ObjectTreeNode::IndexType index);
		ObjectTreeNode::IndexType
			findFirstDirty(ObjectTree &tree, ObjectTreeNode::IndexType index);

		void visit(SceneVisitorContext &context, const SceneNode &node);
		void visit(SceneVisitorContext &context, const CameraNode &node);
		void visit(SceneVisitorContext &context, const TransformNode &node);
		void visit(SceneVisitorContext &context, const PipelineNode &node);
		void visit(SceneVisitorContext &context, const MaterialNode &node);
		void visit(SceneVisitorContext &context,
				   const StaticGeometryNode &node);

		render::DeviceCRef m_device;
		render::SurfaceRef m_surface;

		ContextMap m_contexts;

		std::unordered_map<render::GraphicsPipelineState,
						   render::GraphicsPipelineOwnedPtr>
			m_pipelines;

		struct FrameHolder {
			FrameHolder(render::Surface::Frame &&frame_)
				: frame{std::move(frame_)} {
			}
			render::Surface::Frame frame;
		};

		std::vector<FrameGraphOwnedPtr> m_frame_graphs;
		std::unique_ptr<FrameHolder> m_current_frame;

		// std::optional<render::Surface::Frame> current_frame;

		friend class CameraNode;
		friend class TransformNode;
		friend class MaterialNode;
		friend class PipelineNode;
		friend class SceneNode;
		friend class StaticGeometryNode;
		friend struct SceneVisitorContext;
	};
} // namespace storm::graphics
