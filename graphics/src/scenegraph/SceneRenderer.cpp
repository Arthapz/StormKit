#include <storm/graphics/framegraph/FrameGraph.hpp>
#include <storm/graphics/scenegraph/CameraNode.hpp>
#include <storm/graphics/scenegraph/DrawableManager.hpp>
#include <storm/graphics/scenegraph/MaterialNode.hpp>
#include <storm/graphics/scenegraph/PipelineNode.hpp>
#include <storm/graphics/scenegraph/SceneGraph.hpp>
#include <storm/graphics/scenegraph/SceneGraphObserver.hpp>
#include <storm/graphics/scenegraph/SceneNode.hpp>
#include <storm/graphics/scenegraph/SceneRenderer.hpp>
#include <storm/graphics/scenegraph/StaticGeometryNode.hpp>
#include <storm/graphics/scenegraph/TransformNode.hpp>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Surface.hpp>
#include <storm/render/pipeline/GraphicsPipeline.hpp>
#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/sync/Semaphore.hpp>

#include <storm/log/LogHandler.hpp>

using namespace storm;
using namespace storm::graphics;

using log::operator""_module;

std::string_view colorNodes(std::string_view type) {
	if (type == "SceneNode")
		return "blue";
	else if (type == "CameraNode")
		return "red";
	else if (type == "MaterialNode")
		return "steelblue";
	else if (type == "PipelineNode")
		return "green";
	else if (type == "StaticGeometryNode")
		return "yellow";
	else if (type == "TransformNode")
		return "purple";

	return "blue";
}

namespace storm::graphics {
	struct SceneVisitorContext {
		ObjectTreeNode::IndexType index;
		ObjectTreeNode::DirtyBitType dirty_bits;
		ObjectTreeNode::AccumulatedData &data;

		SceneRenderer::SceneGraphContext &context;
	};
} // namespace storm::graphics

static constexpr auto UBO_SIZE = sizeof(storm::render::Matrix) * 2;

////////////////////////////////////////
////////////////////////////////////////
SceneRenderer::SceneRenderer(render::Surface &surface, render::Device &device)
	: m_device{device}, m_surface{surface} {
	m_frame_graphs.resize(m_surface.get().imageCount());

	for (auto &frame_graph : m_frame_graphs)
		frame_graph = std::make_unique<FrameGraph>(m_device.get());
}

////////////////////////////////////////
////////////////////////////////////////
SceneRenderer::~SceneRenderer() = default;

////////////////////////////////////////
////////////////////////////////////////
SceneRenderer::SceneRenderer(SceneRenderer &&) = default;

////////////////////////////////////////
////////////////////////////////////////
SceneRenderer &SceneRenderer::operator=(SceneRenderer &&) = default;

////////////////////////////////////////
////////////////////////////////////////
std::uint32_t SceneRenderer::prepareRender(SceneGraph &graph) {
	auto it = m_contexts.find(&graph);
	if (it == std::end(m_contexts))
		it = addNewContext(graph);

	auto &context = it->second;
	if (std::size(context.tree->dirties()) > 0) {
		traverse(context);
		context.tree->clearDirties();
	}

	auto frame		= m_surface.get().acquireNextFrame();
	m_current_frame = std::make_unique<FrameHolder>(std::move(frame));

	if (context.per_frame_dirty) {
		auto &transfert_queue = m_device.get().transfertQueue();

		context.transfert_cmb->reset();
		context.transfert_cmb->begin();
		if (context.per_frame_dirty)
			context.transfert_cmb->copyBuffer(*context.per_frame_staging_buffer,
											  *context.per_frame_buffer,
											  UBO_SIZE, 0, 0);
		context.transfert_cmb->end();
		context.transfert_cmb->build();

		auto to_submit = std::array{std::cref(*context.transfert_cmb)};
		auto signal_semaphores =
			std::array{std::cref(*context.transfert_semaphore)};
		transfert_queue.submit(to_submit, {}, signal_semaphores);
	}

	return m_current_frame->frame.image_index;
}

////////////////////////////////////////
////////////////////////////////////////
void SceneRenderer::render(const SceneGraph &graph) {
	auto &context = m_contexts[&graph];
	auto &device  = m_device.get();
	auto &surface = m_surface.get();

	auto &frame		  = m_current_frame->frame;
	auto &frame_graph = *m_frame_graphs[frame.image_index];

	const auto command_buffer = std::cref(frame_graph.commandBuffer());
	auto to_submit			  = std::array{command_buffer};
	auto wait_semaphores	  = std::vector{std::cref(frame.image_available)};
	if (context.per_frame_dirty) {
		wait_semaphores.emplace_back(*context.transfert_semaphore);
		context.per_frame_dirty = 0b0;
	}

	auto signal_semaphores = std::array{std::cref(frame.render_finished)};
	device.graphicsQueue().submit(to_submit, wait_semaphores, signal_semaphores,
								  frame.in_flight);

	surface.present(frame);
}

////////////////////////////////////////
////////////////////////////////////////
void SceneRenderer::genDotFiles(const SceneGraph &graph) {
	m_contexts[&graph].tree->genDotFile("tree.dot", colorNodes);
}

////////////////////////////////////////
////////////////////////////////////////
SceneRenderer::ContextMap::iterator
	SceneRenderer::addNewContext(SceneGraph &graph) {
	auto &device = m_device.get();

	auto it = m_contexts.emplace(&graph, SceneGraphContext{});
	auto &[graph_ptr, context] = *it.first;

	context.tree	  = std::make_unique<ObjectTree>();
	context.observer  = std::make_unique<SceneGraphObserver>(*context.tree);
	context.drawables = std::make_unique<DrawableManager>();

	auto node = ObjectTreeNode{};
	node.setName("SceneNode");

	auto index =
		context.tree->insert(std::move(node), ObjectTreeNode::INVALID_INDEX,
							 ObjectTreeNode::INVALID_INDEX);
	context.tree->markDirty(index, node.dirtyBits());
	context.observer->m_node_links.emplace(index, &graph.rootNode());

	graph.rootNode().setObserver(*context.observer);

	context.per_frame_buffer =
		device.createUniformBuffer(UBO_SIZE * m_surface.get().imageCount(),
								   render::MemoryProperty::Device_Local, true);
	context.per_frame_staging_buffer = device.createStagingBuffer(UBO_SIZE);

	auto &queue					= device.transfertQueue();
	context.transfert_cmb		= queue.createCommandBuffer();
	context.transfert_semaphore = device.createSemaphore();

	return it.first;
}

////////////////////////////////////////
////////////////////////////////////////
void SceneRenderer::traverse(SceneGraphContext &context) {
	auto dirties	 = context.tree->dirties();
	auto dirty_queue = std::queue<ObjectTreeNode::IndexType>{};
	for (auto dirty : dirties) dirty_queue.emplace(dirty);

	while (!std::empty(dirty_queue)) {
		auto dirty = dirty_queue.back();
		dirty_queue.pop();

		auto sub_tree = findFirstDirty(*context.tree, dirty);
		traverseSubTree(context, sub_tree);
	}
}

////////////////////////////////////////
////////////////////////////////////////
void SceneRenderer::traverseSubTree(SceneGraphContext &context,
									ObjectTreeNode::IndexType index) {
	if (index == ObjectTreeNode::INVALID_INDEX)
		return;

	const auto &node_links = context.observer->nodeLinks();
	auto &tree			   = *context.tree;

	log::LogHandler::dlog("SceneRenderer"_module,
						  "traversing subtree with first index {0}", index);

	auto dirty_queue = std::queue<ObjectTreeNode::IndexType>{};
	dirty_queue.emplace(index);

	while (!std::empty(dirty_queue)) {
		auto tree_index = dirty_queue.back();
		dirty_queue.pop();

		auto &tree_node  = tree[tree_index];
		auto &scene_node = node_links.at(tree_index);

		auto accumulated_data = ObjectTreeNode::AccumulatedData{};
		if (tree_node.parent() != ObjectTreeNode::INVALID_INDEX)
			accumulated_data = tree[tree_node.parent()].data();

		auto dirty_bits		 = tree_node.dirtyBits();
		auto visitor_context = SceneVisitorContext{tree_index, dirty_bits,
												   accumulated_data, context};
		scene_node->visit(visitor_context, *this);

		tree_node.setAccumulatedData(std::move(accumulated_data));
		tree_node.setDirtyBits(0x0);

		for (auto child = tree_node.firstChild();
			 child != ObjectTreeNode::INVALID_INDEX;
			 child = tree[child].nextSibling()) {
			if ((dirty_bits & ObjectTreeNode::TRANSFORM_DIRTY_BITS) ==
				ObjectTreeNode::TRANSFORM_DIRTY_BITS)
				tree.markDirty(child, tree[child].dirtyBits() |
										  ObjectTreeNode::TRANSFORM_DIRTY_BITS);
			dirty_queue.emplace(child);
		}
	}
}

////////////////////////////////////////
////////////////////////////////////////
ObjectTreeNode::IndexType
	SceneRenderer::findFirstDirty(ObjectTree &tree,
								  ObjectTreeNode::IndexType index) {
	if (!tree[index].dirtyBits())
		return ObjectTreeNode::INVALID_INDEX;

	auto first = index;

	for (auto current_index = first;
		 current_index != ObjectTreeNode::INVALID_INDEX;
		 current_index = tree[current_index].parent()) {
		if (tree[current_index].dirtyBits())
			first = current_index;
	}

	return first;
}

////////////////////////////////////////
////////////////////////////////////////
void SceneRenderer::visit(SceneVisitorContext &context, const SceneNode &node) {
	if ((context.dirty_bits & ObjectTreeNode::PROJECTION_DIRTY_BITS) ==
		ObjectTreeNode::PROJECTION_DIRTY_BITS) {
		auto data = context.context.per_frame_staging_buffer->map();
		std::memcpy(data, &node.projectionMatrix(), sizeof(render::Matrix));
		context.context.per_frame_staging_buffer->flush(0,
														sizeof(render::Matrix));
		context.context.per_frame_staging_buffer->unmap();
		context.context.per_frame_dirty |=
			ObjectTreeNode::PROJECTION_DIRTY_BITS;
	}
}

////////////////////////////////////////
////////////////////////////////////////
void SceneRenderer::visit(SceneVisitorContext &context,
						  const CameraNode &node) {
	if ((context.dirty_bits & ObjectTreeNode::VIEW_DIRTY_BITS) ==
		ObjectTreeNode::VIEW_DIRTY_BITS) {
		auto data = context.context.per_frame_staging_buffer->map(
			sizeof(render::Matrix));
		std::memcpy(data, &node.viewMatrix(), sizeof(render::Matrix));
		context.context.per_frame_staging_buffer->flush(sizeof(render::Matrix),
														sizeof(render::Matrix));
		context.context.per_frame_staging_buffer->unmap();
		context.context.per_frame_dirty |= ObjectTreeNode::VIEW_DIRTY_BITS;
	}
}

////////////////////////////////////////
////////////////////////////////////////
void SceneRenderer::visit(SceneVisitorContext &context,
						  const TransformNode &node) {
	if ((context.dirty_bits & ObjectTreeNode::TRANSFORM_DIRTY_BITS) ==
		ObjectTreeNode::TRANSFORM_DIRTY_BITS)
		context.data.transform *= node.transform();
}

////////////////////////////////////////
////////////////////////////////////////
void SceneRenderer::visit(SceneVisitorContext &context,
						  const PipelineNode &node) {
	context.data.state = node.state();
}

////////////////////////////////////////
////////////////////////////////////////
void SceneRenderer::visit(SceneVisitorContext &context,
						  const MaterialNode &node) {
	context.data.material = node.material();
}

////////////////////////////////////////
////////////////////////////////////////
void SceneRenderer::visit(SceneVisitorContext &context,
						  const StaticGeometryNode &node) {
	if (!context.context.drawables->hasDrawable(
			context.data.state, context.data.material, context.index)) {
		context.context.drawables->addDrawable(
			context.data.state, context.data.material,
			{context.index, node.vertexArray(), node.indexArray(),
			 context.data.transform});
	} else {
	}
}
