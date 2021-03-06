#include <storm/engine/render/3D/PbrMesh.hpp>

using namespace storm;
using namespace storm::engine;

/////////////////////////////////////
/////////////////////////////////////
PbrMesh::PbrMesh() noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
PbrMesh::PbrMesh(std::string name) : Drawable { std::move(name) } {
}

/////////////////////////////////////
/////////////////////////////////////
PbrMesh::~PbrMesh() = default;

/////////////////////////////////////
/////////////////////////////////////
PbrMesh::PbrMesh(const PbrMesh &) = default;

/////////////////////////////////////
/////////////////////////////////////
auto PbrMesh::operator=(const PbrMesh &) -> PbrMesh & = default;

/////////////////////////////////////
/////////////////////////////////////
PbrMesh::PbrMesh(PbrMesh &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto PbrMesh::operator=(PbrMesh &&) noexcept -> PbrMesh & = default;

/////////////////////////////////////
/////////////////////////////////////
auto PbrMesh::addAnimation(Animation animation) -> void {
    m_animations.emplace_back(std::move(animation));
}

/////////////////////////////////////
/////////////////////////////////////
auto PbrMesh::addSkin(Skin skin) -> core::UInt32 {
    const auto index = std::size(m_skins);

    m_skins.emplace_back(std::move(skin));

    return index;
}

/////////////////////////////////////
/////////////////////////////////////
auto PbrMesh::addNode(Node node) -> Node::ID {
    const auto index = std::size(m_nodes);

    if (node.parent == Node::INVALID) m_root_nodes.emplace_back(index);

    m_nodes.emplace_back(std::move(node));

    return index;
}
