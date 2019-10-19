#include <storm/graphics/scenegraph/SceneRenderer.hpp>
#include <storm/graphics/scenegraph/StaticGeometryNode.hpp>

using namespace storm;
using namespace storm::graphics;

////////////////////////////////////////
////////////////////////////////////////
StaticGeometryNode::StaticGeometryNode(std::string name)
	: Node{"StaticGeometryNode"} {
	setName(std::move(name));
}

////////////////////////////////////////
////////////////////////////////////////
StaticGeometryNode::~StaticGeometryNode() = default;

////////////////////////////////////////
////////////////////////////////////////
StaticGeometryNode::StaticGeometryNode(StaticGeometryNode &&) = default;

////////////////////////////////////////
////////////////////////////////////////
StaticGeometryNode &StaticGeometryNode::
	operator=(StaticGeometryNode &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void StaticGeometryNode::visit(SceneVisitorContext &context,
							   SceneRenderer &renderer) const {
	renderer.visit(context, *this);
}
