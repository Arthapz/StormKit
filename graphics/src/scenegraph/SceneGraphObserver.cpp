#include <storm/graphics/scenegraph/SceneGraphObserver.hpp>

#include <storm/log/LogHandler.hpp>

using namespace storm;
using namespace storm::graphics;

using log::operator""_module;

////////////////////////////////////////
////////////////////////////////////////
SceneGraphObserver::SceneGraphObserver(ObjectTree &object_tree)
	: m_object_tree{object_tree} {
}

////////////////////////////////////////
////////////////////////////////////////
SceneGraphObserver::~SceneGraphObserver() = default;

////////////////////////////////////////
////////////////////////////////////////
SceneGraphObserver::SceneGraphObserver(SceneGraphObserver &&) = default;

////////////////////////////////////////
////////////////////////////////////////
SceneGraphObserver &SceneGraphObserver::
	operator=(SceneGraphObserver &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void SceneGraphObserver::onNotified(NodeEvent event,
									NodePayloadVariant &&payload) {
	if (event == NodeEvent::Updated)
		onUpdated(std::get<UpdatedPayload>(payload));
	if (event == NodeEvent::ChildAdded)
		onChildNodeAdded(std::get<ChildAddedPayload>(payload));
	else if (event == NodeEvent::ChildRemoved)
		onChildNodeRemoved(std::get<ChildRemovedPayload>(payload));
}

////////////////////////////////////////
////////////////////////////////////////
void SceneGraphObserver::onChildNodeAdded(ChildAddedPayload &payload) {
	auto &tree = m_object_tree.get();

	auto &parent = payload.parent.get();
	auto &child  = payload.child.get();

	log::LogHandler::dlog("SceneGraph"_module, "{} ({}) added {} child ({})",
						  parent.name(), parent.identifier(), child.name(),
						  child.identifier());

	auto parent_tree_nodes = std::vector<ObjectTreeNode::IndexType>{};
	for (const auto &[index, node] : m_node_links)
		if (&parent == node)
			parent_tree_nodes.emplace_back(index);

	auto child_tree_nodes = std::vector<ObjectTreeNode::IndexType>{};
	child_tree_nodes.reserve(std::size(parent_tree_nodes));

	for (auto parent_tree_id : parent_tree_nodes) {
		auto tree_node = ObjectTreeNode{};
		tree_node.setName(fmt::format("{}", child.identifier()));

		const auto child_tree_id =
			tree.insert(std::move(tree_node), parent_tree_id,
						core::TreeNode::INVALID_INDEX);

		child_tree_nodes.emplace_back(child_tree_id);

		tree.markDirty(parent_tree_id,
					   tree[parent_tree_id].dirtyBits() |
						   ObjectTreeNode::CHILD_ADDED_DIRTY_BITS);
	}

	for (const auto &index : child_tree_nodes) m_node_links[index] = &child;
}

////////////////////////////////////////
////////////////////////////////////////
void SceneGraphObserver::onChildNodeRemoved(ChildRemovedPayload &payload) {
	const auto &parent = payload.parent.get();
	const auto &child  = payload.child.get();

	log::LogHandler::dlog("SceneGraph"_module, "{} ({}) removed {} child ({})",
						  parent.name(), parent.identifier(), child.name(),
						  child.identifier());
}

////////////////////////////////////////
////////////////////////////////////////
void SceneGraphObserver::onUpdated(UpdatedPayload &payload) {
	auto &tree = m_object_tree.get();
	auto &node = payload.node.get();

	for (const auto &[index, node_] : m_node_links)
		if (&node == node_)
			tree.markDirty(index, tree[index].dirtyBits() | payload.dirty_bits);
}
