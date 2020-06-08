/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/render/sync/Fence.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/core/Transform.hpp>

#include <storm/engine/drawable/3D/Mesh.hpp>
#include <storm/engine/drawable/3D/MeshNode.hpp>
#include <storm/engine/drawable/3D/Model.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
Mesh::Mesh(Engine &engine, const Material &material, std::string name)
    : Drawable { engine }, m_name { std::move(name) }, m_material { &material } {
    m_transform = std::make_unique<engine::Transform>(*m_engine);
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
    STORM_EXPECTS(m_root_node == core::TreeNode::INVALID_INDEX &&
                  parent == core::TreeNode::INVALID_INDEX);

    auto tree_node = core::TreeNode {};
    auto index     = m_tree.insert(std::move(tree_node), parent, core::TreeNode::INVALID_INDEX);

    auto &_node = m_nodes.emplace_back(std::move(node));

    m_node_link[index] = core::makeObserver(_node);

    if (m_root_node == core::TreeNode::INVALID_INDEX) m_root_node = index;

    return index;
}

////////////////////////////////////////
////////////////////////////////////////
const MeshNode &Mesh::getNode(core::TreeNode::IndexType id) const noexcept {
    STORM_EXPECTS(id != core::TreeNode::INVALID_INDEX);

    const auto it = std::find_if(std::cbegin(m_node_link),
                                 std::cend(m_node_link),
                                 [&id](const auto &pair) { return pair.first == id; });

    STORM_EXPECTS(it != std::cend(m_node_link));

    return *it->second;
}

////////////////////////////////////////
////////////////////////////////////////
void Mesh::render(render::CommandBuffer &cmb,
                  const render::RenderPass &pass,
                  std::vector<BindableBaseConstObserverPtr> bindables,
                  render::GraphicsPipelineState state) {
    m_transform->flush();
    bindables.emplace_back(core::makeConstObserver(m_transform));

    state.shader_state = m_material->m_data.shader_state;

    cmb.bindVertexBuffers({ *m_vertex_buffer }, { 0u });
    if (m_index_buffer) { cmb.bindIndexBuffer(*m_index_buffer, 0, m_large_indices); }

    for (auto &node : m_nodes) { node.render(cmb, pass, bindables, state); }
}

////////////////////////////////////////
////////////////////////////////////////
void Mesh::bake() {
    auto &device = m_engine->device();

    auto vertices = std::vector<core::Byte> {};
    auto indices  = std::vector<core::Byte> {};

    auto nodes = std::deque<core::TreeNode::IndexType> {};
    nodes.emplace_back(m_root_node);

    auto vertex_it = 0u;
    auto index_it  = 0u;
    while (!std::empty(nodes)) {
        auto node_it = nodes.front();
        nodes.pop_front();

        const auto &tree_node = m_tree[node_it];
        auto &node            = m_node_link[node_it];

        for (auto &primitive : node->primitives()) {
            constexpr auto vertex_size = sizeof(Model::Vertex);
            const auto index_size =
                (primitive.large_indices) ? sizeof(core::UInt32) : sizeof(core::UInt16);
            m_large_indices |= primitive.large_indices;

            primitive.first_vertex = vertex_it;
            primitive.first_index  = index_it;

            vertices.resize(std::size(vertices) + vertex_size * primitive.vertex_count);
            indices.resize(std::size(indices) + index_size * primitive.index_count);

            core::ranges::copy(primitive.vertices.bytes(),
                               std::begin(vertices) + vertex_it * vertex_size);

            if (primitive.index_count > 0u) {
                std::visit(
                    [&indices, index_it](const auto &index_array) {
                        STORM_EXPECTS(!std::empty(index_array));

                        using IndexArrayType     = std::remove_reference_t<decltype(index_array)>;
                        using IndexArrayTypeType = typename IndexArrayType::value_type;

                        core::ranges::copy(index_array,
                                           reinterpret_cast<IndexArrayTypeType *>(
                                               std::data(indices)) +
                                               index_it);
                    },
                    primitive.indices);
            }

            vertex_it += primitive.vertex_count;
            index_it += primitive.index_count;
        }

        auto child = tree_node.firstChild();
        while (child != core::TreeNode::INVALID_INDEX) {
            nodes.emplace_front(child);

            const auto &child_node = m_tree[child];
            child                  = child_node.nextSibling();
        }
    }

    auto staging_buffer = device.createStagingBuffer(std::size(vertices) + std::size(indices));
    staging_buffer.upload<core::Byte>(vertices);
    if (!std::empty(indices)) staging_buffer.upload<core::Byte>(indices, std::size(vertices));

    m_vertex_buffer = device.createVertexBufferPtr(std::size(vertices),
                                                   render::MemoryProperty::Device_Local,
                                                   true);
    if (!std::empty(indices))
        m_index_buffer = device.createIndexBufferPtr(std::size(indices),
                                                     render::MemoryProperty::Device_Local,
                                                     true);

    auto cmb = device.graphicsQueue().createCommandBuffer();
    cmb.begin(true);
    cmb.copyBuffer(staging_buffer, *m_vertex_buffer, std::size(vertices));
    if (!std::empty(indices))
        cmb.copyBuffer(staging_buffer, *m_index_buffer, std::size(indices), std::size(vertices));
    cmb.end();

    cmb.build();

    auto fence = device.createFence();
    cmb.submit({}, {}, core::makeObserver(fence));

    fence.wait();
}

////////////////////////////////////////
////////////////////////////////////////
MeshOwnedPtr Mesh::clonePtr() const {
    auto mesh = clone();
    return std::make_unique<Mesh>(std::move(mesh));
}

////////////////////////////////////////
////////////////////////////////////////
Mesh Mesh::clone() const {
    auto mesh       = Mesh { *m_engine, *m_material, m_name };
    auto &transform = mesh.transform();
    transform.setPosition(m_transform->position());
    transform.setScale(m_transform->scale());
    transform.setOrientation(m_transform->orientationEuler());

    auto nodes = std::deque<core::TreeNode::IndexType> {};
    nodes.emplace_back(m_root_node);

    while (!std::empty(nodes)) {
        auto node_it = nodes.front();
        nodes.pop_front();

        const auto &tree_node = m_tree[node_it];
        const auto &node      = m_node_link.at(node_it);

        auto copy = MeshNode { *m_engine, std::string { node->name() } };
        copy.setTransform(node->transform());

        for (const auto &primitive : node->primitives()) {
            auto material_instance = primitive.material_instance->clone();

            auto copy_primitive =
                MeshPrimitive { .name              = primitive.name,
                                .vertices          = primitive.vertices,
                                .indices           = primitive.indices,
                                .attributes        = primitive.attributes,
                                .bindings          = primitive.bindings,
                                .first_vertex      = primitive.first_vertex,
                                .vertex_count      = primitive.vertex_count,
                                .first_index       = primitive.first_index,
                                .index_count       = primitive.index_count,
                                .large_indices     = primitive.large_indices,
                                .bounding_box      = primitive.bounding_box,
                                .material_instance = std::move(material_instance) };

            copy.addPrimitive(std::move(copy_primitive));
        }

        mesh.addNode(std::move(copy), tree_node.parent());

        auto child = tree_node.firstChild();
        while (child != core::TreeNode::INVALID_INDEX) {
            nodes.emplace_front(child);

            const auto &child_node = m_tree[child];
            child                  = child_node.nextSibling();
        }
    }

    return mesh;
}

////////////////////////////////////////
////////////////////////////////////////
void Mesh::recomputeBoundingBox() const noexcept {
    m_bounding_box.min = { 0.f, 0.f, 0.f };
    m_bounding_box.max = { 0.f, 0.f, 0.f };

    auto nodes = std::deque<core::TreeNode::IndexType> {};
    nodes.emplace_back(m_root_node);

    while (!std::empty(nodes)) {
        auto node_it = nodes.front();
        nodes.pop_front();

        const auto &tree_node    = m_tree[node_it];
        const auto &node         = m_node_link.at(node_it);
        const auto &bounding_box = node->boundingBox();

        m_bounding_box.min = core::min(m_bounding_box.min, bounding_box.min);
        m_bounding_box.max = core::max(m_bounding_box.max, bounding_box.max);

        auto child = tree_node.firstChild();
        while (child != core::TreeNode::INVALID_INDEX) {
            nodes.emplace_front(child);

            const auto &child_node = m_tree[child];
            child                  = child_node.nextSibling();
        }
    }

    const auto extent = m_bounding_box.max - m_bounding_box.min;

    m_bounding_box.extent.width  = extent.x;
    m_bounding_box.extent.height = extent.y;
    m_bounding_box.extent.depth  = extent.z;
}
