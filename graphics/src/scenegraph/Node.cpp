#include <gsl/gsl_assert>
#include <storm/graphics/scenegraph/Node.hpp>

#include <algorithm>

using namespace storm;
using namespace storm::graphics;

////////////////////////////////////////
////////////////////////////////////////
Node::Node(std::string identifier) : m_identifier{std::move(identifier)} {
}

////////////////////////////////////////
////////////////////////////////////////
Node::~Node() = default;

////////////////////////////////////////
////////////////////////////////////////
Node::Node(Node &&) = default;

////////////////////////////////////////
////////////////////////////////////////
Node &Node::operator=(Node &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void Node::addChild(Node &child) {
	notify(NodeEvent::ChildAdded, ChildAddedPayload{*this, child});

	m_childs.emplace_back(child);

	if (hasObserver())
		child.setObserver(observer());

	child.addParent(*this);
}

////////////////////////////////////////
////////////////////////////////////////
void Node::removeChild(Node &child, bool all) {
	notify(NodeEvent::ChildRemoved, ChildRemovedPayload{*this, child});

	static const auto predicate = [&child](const NodeRef &node) {
		return &node.get() == &child;
	};

	const auto begin = std::cbegin(m_childs);
	const auto end   = std::cend(m_childs);
	if (all) {
		for (auto it = std::find_if(begin, end, predicate); it != end;
			 it		 = std::find_if(begin, end, predicate)) {
			it->get().removeParent(*this, true);
			m_childs.erase(it);
		}
	} else {
		auto it = std::find_if(begin, end, predicate);

		if (it != end) {
			it->get().removeParent(*this, false);
			m_childs.erase(it);
		}
	}
}

////////////////////////////////////////
////////////////////////////////////////
void Node::addParent(const Node &parent) {
	m_parents.emplace_back(parent);
}

////////////////////////////////////////
////////////////////////////////////////
void Node::removeParent(const Node &parent, bool all) {
	static const auto predicate = [&parent](const NodeCRef &node) {
		return &node.get() == &parent;
	};

	const auto begin = std::cbegin(m_parents);
	const auto end   = std::cend(m_parents);

	if (all) {
		for (auto it = std::find_if(begin, end, predicate); it != end;
			 it		 = std::find_if(begin, end, predicate)) {
			m_parents.erase(it);
		}
	} else {
		auto it = std::find_if(begin, end, predicate);

		if (it != end) {
			m_parents.erase(it);
		}
	}

	if (std::empty(m_parents))
		resetObserver();
}

////////////////////////////////////////
////////////////////////////////////////
void Node::setObserver(NodeSubject::ObserverType &graph) {
	registerObserver(graph);

	notifyDefferedEvents();

	for (auto &child : m_childs) child.get().setObserver(graph);
}
