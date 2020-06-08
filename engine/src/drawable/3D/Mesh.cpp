/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/render/sync/Fence.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/drawable/3D/Mesh.hpp>
#include <storm/engine/drawable/3D/MeshNode.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
Mesh::Mesh(Engine &engine, const Material &material, std::string name)
    : Drawable { engine }, m_name { std::move(name) }, m_material { &material } {
    auto &device = m_engine->device();
}

////////////////////////////////////////
////////////////////////////////////////
Mesh::~Mesh() = default;

////////////////////////////////////////
////////////////////////////////////////
Mesh::Mesh(Mesh &&) = default;

////////////////////////////////////////
////////////////////////////////////////
Mesh &Mesh::operator=(Mesh &&) = default;

////////////////////////////////////////
////////////////////////////////////////
core::TreeNode::IndexType Mesh::addNode(MeshNode &&node, core::TreeNode::IndexType parent) {
    auto index = m_nodes.insert(std::move(node), parent, MeshNode::INVALID_INDEX);

    return index;
}

////////////////////////////////////////
////////////////////////////////////////
const MeshNode &Mesh::getNode(core::TreeNode::IndexType id) const noexcept {
    return m_nodes[id];
}

////////////////////////////////////////
////////////////////////////////////////
void Mesh::bake() {
}

////////////////////////////////////////
////////////////////////////////////////
MeshOwnedPtr Mesh::clonePtr() const {
}

////////////////////////////////////////
////////////////////////////////////////
Mesh Mesh::clone() const {
}
