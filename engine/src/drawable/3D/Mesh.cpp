/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/render/sync/Fence.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/core/RingHardwareBuffer.hpp>
#include <storm/engine/core/Transform.hpp>

#include <storm/engine/drawable/3D/Mesh.hpp>
#include <storm/engine/drawable/3D/Model.hpp>
#include <storm/engine/drawable/3D/SubMesh.hpp>

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
core::UInt32 Mesh::addSkin(Mesh::Skin &&skin) {
    const auto index = std::size(m_skins);

    m_skins.emplace_back(std::move(skin));

    return index;
}

////////////////////////////////////////
////////////////////////////////////////
Mesh::Node::ID Mesh::addNode(Node &&node) {
    const auto index = std::size(m_nodes);

    if (node.parent == Node::INVALID) m_root_nodes.emplace_back(index);

    const auto &_node = m_nodes.emplace_back(std::move(node));

    if (_node.submesh) m_submesh_nodes.emplace_back(index);

    return index;
}

////////////////////////////////////////
////////////////////////////////////////
const Mesh::Node &Mesh::getNode(Node::ID id) const noexcept {
    STORM_EXPECTS(id < std::size(m_nodes));

    return m_nodes[id];
}

////////////////////////////////////////
////////////////////////////////////////
void Mesh::render(render::CommandBuffer &cmb,
                  const render::RenderPass &pass,
                  std::vector<BindableBaseConstObserverPtr> bindables,
                  render::GraphicsPipelineState state,
                  float delta_time) {
    m_transform->flush();

    if (m_current_animation) updateAnimation(delta_time);

    if (m_need_to_update_nodes) {
        updateNodes();
        m_need_to_update_nodes = false;
    }

    bindables.emplace_back(core::makeConstObserver(m_transform));

    state.shader_state = m_material->m_data.shader_state;

    cmb.bindVertexBuffers({ *m_vertex_buffer }, { 0u });
    if (m_index_buffer) { cmb.bindIndexBuffer(*m_index_buffer, 0, m_large_indices); }

    auto i = 0u;
    for (auto &node_id : m_submesh_nodes) {
        auto &node    = m_nodes[node_id];
        auto &submesh = node.submesh.value();

        auto _bindables = bindables;
        _bindables.emplace_back(this);

        m_offset = m_mesh_data_buffer->currentOffset() + i++ * sizeof(Data);

        submesh.render(cmb, pass, std::move(_bindables), state);
    }
}

////////////////////////////////////////
////////////////////////////////////////
void Mesh::bake() {
    const auto buffering_count = m_engine->surface().bufferingCount();
    const auto &device         = m_engine->device();
    const auto &pool           = m_engine->descriptorPool();

    auto vertices = std::vector<core::Byte> {};
    auto indices  = std::vector<core::Byte> {};

    auto nodes = std::deque<Node::ID> {};
    for (const auto node_id : m_root_nodes) nodes.emplace_back(node_id);

    constexpr auto vertex_size = sizeof(Model::Vertex);

    auto vertex_it = 0u;
    auto index_it  = 0u;
    while (!std::empty(nodes)) {
        auto node_it = nodes.front();
        nodes.pop_front();

        auto &node = m_nodes[node_it];
        for (const auto child_id : node.childs) nodes.emplace_front(child_id);

        if (!node.submesh) continue;

        for (auto &primitive : node.submesh.value().primitives()) {
            m_large_indices |= primitive.large_indices;

            primitive.first_vertex = vertex_it;
            primitive.first_index  = index_it;

            vertices.resize(std::size(vertices) + std::size(primitive.vertices));

            core::ranges::copy(primitive.vertices.data(),
                               std::begin(vertices) + vertex_it * vertex_size);

            if (primitive.index_count > 0u) {
                std::visit(
                    [&indices, index_it](const auto &index_array) {
                        STORM_EXPECTS(!std::empty(index_array));

                        using IndexArrayType     = std::remove_reference_t<decltype(index_array)>;
                        using IndexArrayTypeType = typename IndexArrayType::value_type;

                        indices.resize(std::size(indices) +
                                       std::size(index_array) * sizeof(IndexArrayTypeType));
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

    if (!std::empty(m_name)) {
        device.setObjectName(*m_vertex_buffer, fmt::format("{}:VertexBuffer", m_name));
        if (m_index_buffer)
            device.setObjectName(*m_index_buffer, fmt::format("{}:IndexBuffer", m_name));
    }

    m_mesh_data_buffer =
        std::make_unique<RingHardwareBuffer>((!std::empty(m_skins)) ? buffering_count : 1u,
                                             device,
                                             render::HardwareBufferUsage::Uniform,
                                             sizeof(Data) * std::size(m_submesh_nodes));

    if (!std::empty(m_name)) {
        device.setObjectName(m_mesh_data_buffer->buffer(), fmt::format("{}:SkinBuffer", m_name));
    }

    m_descriptor_sets = pool.allocateDescriptorSetsPtr(1, descriptorLayout());
    if (!std::empty(m_name)) {
        for (auto i = 0u; i < std::size(m_descriptor_sets); ++i)
            device.setObjectName(*m_descriptor_sets[i],
                                 fmt::format("{}:SkinDescriptorSet{}", m_name, i));
    }

    const auto descriptor =
        render::BufferDescriptor { .type    = render::DescriptorType::Uniform_Buffer_Dynamic,
                                   .binding = 0u,
                                   .buffer  = core::makeConstObserver(m_mesh_data_buffer->buffer()),
                                   .range   = sizeof(Data),
                                   .offset  = 0 };
    const auto descriptors =
        render::DescriptorStaticArray<1> { render::Descriptor { std::move(descriptor) } };

    descriptorSet().update(descriptors);

    m_need_to_update_nodes = true;
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

    for (const auto &node : m_nodes) {
        auto copy_node        = Mesh::Node {};
        copy_node.translation = node.translation;
        copy_node.rotation    = node.rotation;
        copy_node.scale       = node.scale;
        copy_node.matrix      = node.matrix;
        copy_node.parent      = node.parent;
        copy_node.childs      = node.childs;
        copy_node.skin        = node.skin;

        if (node.submesh) {
            const auto &submesh = node.submesh.value();

            copy_node.submesh = SubMesh { *m_engine, std::string { submesh.name() } };

            for (const auto &primitive : submesh.primitives()) {
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

                copy_node.submesh.value().addPrimitive(std::move(copy_primitive));
            }
        }

        mesh.addNode(std::move(copy_node));
    }

    for (const auto &skin : m_skins) mesh.addSkin(Skin { skin });
    for (const auto &animation : m_animations) mesh.addAnimation(Animation { animation });

    mesh.bake();

    return mesh;
}

////////////////////////////////////////
////////////////////////////////////////
void Mesh::addAnimation(Animation &&animation) {
    m_animations.emplace_back(std::move(animation));
}

////////////////////////////////////////
////////////////////////////////////////
void Mesh::playAnimation(core::UInt32 id, bool loop) {
    if (std::empty(m_animations) || id > std::size(m_animations)) {
        log::LogHandler::wlog("There is no animation {} on mesh {}", id, m_name);
        return;
    }

    m_current_animation = core::makeObserver(m_animations[id]);
    m_loop_animation    = loop;
}

////////////////////////////////////////
////////////////////////////////////////
void Mesh::playAnimation(std::string_view name, bool loop) {
    auto it = core::ranges::find_if(m_animations, [&name](const auto &animation) {
        return name == animation.name;
    });

    if (it == core::ranges::end(m_animations)) {
        log::LogHandler::wlog("There is no animation {} on mesh {}", name, m_name);
        return;
    }

    m_current_animation = core::makeObserver(*it);
    m_loop_animation    = loop;
}

////////////////////////////////////////
////////////////////////////////////////
void Mesh::recomputeBoundingBox() const noexcept {
    m_bounding_box.min = { 0.f, 0.f, 0.f };
    m_bounding_box.max = { 0.f, 0.f, 0.f };

    for (const auto node_id : m_submesh_nodes) {
        const auto &node    = m_nodes[node_id];
        const auto &submesh = node.submesh.value();

        const auto &bounding_box = submesh.boundingBox();

        m_bounding_box.min = core::min(m_bounding_box.min, bounding_box.min);
        m_bounding_box.max = core::max(m_bounding_box.max, bounding_box.max);
    }

    const auto extent = m_bounding_box.max - m_bounding_box.min;

    m_bounding_box.extent.width  = extent.x;
    m_bounding_box.extent.height = extent.y;
    m_bounding_box.extent.depth  = extent.z;
}

////////////////////////////////////////
////////////////////////////////////////
void Mesh::updateNodes() {
    m_mesh_data_buffer->next();

    auto j = 0u;
    for (const auto node_id : m_submesh_nodes) {
        const auto &node = m_nodes[node_id];

        const auto transform = node.globalTransform(m_nodes);

        auto data      = Data {};
        data.transform = transform;

        if (node.skin != Node::NO_SKIN) {
            const auto inverse_transform = core::inverse(transform);
            const auto &skin             = m_skins[node.skin];
            const auto joint_count       = std::size(skin.joints);

            data.joint_count = joint_count;

            for (auto i = 0u; i < joint_count; ++i) {
                const auto &joint = m_nodes[skin.joints[i]];

                data.joints[i] = joint.globalTransform(m_nodes) * skin.inverse_bind_matrices[i];
                data.joints[i] = inverse_transform * data.joints[i];
            }
        }

        m_mesh_data_buffer->upload<Data>(core::makeConstSpan<Data>(data), j++ * sizeof(Data));
    }
}

////////////////////////////////////////
////////////////////////////////////////
void Mesh::updateAnimation(float delta_time) {
    m_current_animation->current_time += Animation::Seconds { delta_time };

    if (m_current_animation->current_time > m_current_animation->end) {
        if (!m_loop_animation) {
            m_current_animation = nullptr;
            return;
        } else {
            m_current_animation->current_time -= m_current_animation->end;
        }
    }

    const auto current_time = m_current_animation->current_time.count();
    for (auto &channel : m_current_animation->channels) {
        auto &sampler = m_current_animation->samplers[channel.sampler];

        for (auto i = 0u; i < std::size(sampler.inputs) - 1u; ++i) {
            if ((current_time >= sampler.inputs[i]) && (current_time <= sampler.inputs[i + 1u])) {
                const auto interpolation = std::max(0.f, current_time - sampler.inputs[i]) /
                                           (sampler.inputs[i + 1u] - sampler.inputs[i]);

                auto &node = m_nodes[channel.node];

                if (channel.path == Path::Translation)
                    node.translation =
                        core::mix(sampler.outputs[i], sampler.outputs[i + 1u], interpolation);
                else if (channel.path == Path::Rotation) {
                    auto q1 = core::Quaternionf {};
                    q1.x    = sampler.outputs[i].x;
                    q1.y    = sampler.outputs[i].y;
                    q1.z    = sampler.outputs[i].z;
                    q1.w    = sampler.outputs[i].w;

                    auto q2 = core::Quaternionf {};
                    q2.x    = sampler.outputs[i + 1u].x;
                    q2.y    = sampler.outputs[i + 1u].y;
                    q2.z    = sampler.outputs[i + 1u].z;
                    q2.w    = sampler.outputs[i + 1u].w;

                    node.rotation = core::normalize(core::slerp(q1, q2, interpolation));
                } else if (channel.path == Path::Scale)
                    node.scale =
                        core::mix(sampler.outputs[i], sampler.outputs[i + 1u], interpolation);
            }
        }
    }

    m_need_to_update_nodes = true;
}
