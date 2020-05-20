/////////// - StormKit::log - ///////////
#include <storm/log/LogHandler.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/scene/Scene.hpp>

#include <storm/engine/drawable/Mesh.hpp>
#include <storm/engine/drawable/Model.hpp>
#include <storm/engine/drawable/SubMesh.hpp>

#include <storm/engine/material/PBRMaterial.hpp>
#include <storm/engine/material/PBRMaterialInstance.hpp>

/////////// - Tinygltf - ///////////
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_USE_CPP14
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>

using namespace storm;
using namespace storm::engine;

using storm::log::operator""_module;
static constexpr auto LOG_MODULE = "engine"_module;

constexpr render::Format toStormKit(int type) noexcept {
    switch (type) {
        case TINYGLTF_TYPE_SCALAR: return render::Format::Float;
        case TINYGLTF_TYPE_VEC2: return render::Format::Float2;
        case TINYGLTF_TYPE_VEC3: return render::Format::Float3;
        case TINYGLTF_TYPE_VEC4: return render::Format::Float4;
    }

    return render::Format::Float;
}

namespace storm::engine::v2 {
    ////////////////////////////////////////
    ////////////////////////////////////////
    Model::Model(Engine &engine, TexturePool &texture_pool, MaterialPool &material_pool)
        : m_engine { &engine }, m_texture_pool { &texture_pool }, m_material_pool {
              &material_pool
          } {}

    ////////////////////////////////////////
    ////////////////////////////////////////
    Model::~Model() = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    Model::Model(Model &&) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    Model &Model::operator=(Model &&) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    void Model::load(const std::filesystem::path &path) {
        m_filepath = std::filesystem::absolute(path);

        auto loader = tinygltf::TinyGLTF {};

        auto model = tinygltf::Model {};

        auto error   = std::string {};
        auto warning = std::string {};

        auto res = false;

        if (m_filepath.string().back() == 'b' || m_filepath.string().back() == 'B')
            res = loader.LoadBinaryFromFile(&model, &error, &warning, path.string());
        else
            res = loader.LoadASCIIFromFile(&model, &error, &warning, path.string());
        if (!res) {
            if (!std::empty(error)) log::LogHandler::elog(LOG_MODULE, "{}", error);
            if (!std::empty(warning)) log::LogHandler::elog(LOG_MODULE, "{}", warning);

            return;
        }

        auto scene = model.scenes[model.defaultScene];

        auto gltf_meshes =
            std::vector<std::pair<_std::observer_ptr<const tinygltf::Mesh>, core::Matrixf>> {};

        const auto getNodeMatrix = [](const tinygltf::Node &node) {
            auto matrix = core::Matrixf { 1.f };

            auto translation = core::Vector3f { 0.f };
            auto rotation    = core::Matrixf { 1.f };
            auto scale       = core::Vector3f { 1.f };

            if (!std::empty(node.matrix)) matrix = core::make_mat4(std::data(node.matrix));
            if (!std::empty(node.translation))
                translation = core::make_vec3(std::data(node.translation));
            if (!std::empty(node.rotation)) {
                const auto quat = core::Quaternionf { core::make_quat(std::data(node.rotation)) };
                rotation        = core::Matrixf { std::move(quat) };
            }
            if (!std::empty(node.scale)) scale = core::make_vec3(std::data(node.scale));

            matrix = core::translate(core::Matrixf { 1.f }, translation) * rotation *
                     core::scale(core::Matrixf { 1.f }, scale) * matrix;

            return matrix;
        };

        auto node_queue = std::deque<std::pair<tinygltf::Node *, core::Matrixf>> {};
        for (const auto &node_id : scene.nodes) {
            auto &node = model.nodes[node_id];

            auto matrix = getNodeMatrix(node);

            node_queue.emplace_front(&node, std::move(matrix));
        }

        while (!std::empty(node_queue)) {
            auto [node, matrix] = node_queue.front();
            node_queue.pop_front();

            if (!std::empty(node->children)) {
                for (auto child_id : node->children) {
                    auto &child = model.nodes[child_id];

                    auto child_matrix = matrix * getNodeMatrix(child);

                    node_queue.emplace_front(&child, std::move(child_matrix));
                }
            }

            if (node->mesh >= 0) {
                gltf_meshes.emplace_back(core::makeConstObserver(model.meshes[node->mesh]),
                                         std::move(matrix));
            }
        }

        for (const auto &[gltf_mesh, matrix] : gltf_meshes) {
            auto &mesh             = m_meshes.emplace_back(doParseMesh(model, *gltf_mesh));
            mesh.initial_transform = std::move(matrix);
        }

        m_loaded = true;
    } // namespace storm::engine::v2

    ////////////////////////////////////////
    ////////////////////////////////////////
    MeshArray Model::createMeshes() noexcept {
        STORM_EXPECTS(m_loaded);

        auto &material = *m_material_pool->get(Scene::DEFAULT_PBR_MATERIAL_NAME);

        auto meshes = MeshArray {};

        for (auto &_mesh : m_meshes) {
            auto mesh = m_engine->createMesh(material);

            mesh.setVertexArray(_mesh.vertex_array, _mesh.attributes, _mesh.bindings);
            if (_mesh.has_indices) mesh.setIndexArray(_mesh.index_array);

            mesh.transform().setMatrix(_mesh.initial_transform);

            for (auto _submesh : _mesh.submeshes) {
                auto &submesh = mesh.addSubmesh(_submesh.vertex_count,
                                                _submesh.first_index,
                                                _submesh.index_count,
                                                _submesh.min,
                                                _submesh.max);

                auto &material = submesh.materialInstance();

                for (const auto &[name, sampler] : _submesh.samplers) {
                    material.setSamplerTexture(name, *sampler);
                }

                for (const auto &[name, data] : _submesh.datas) {
                    material.setRawDataValue(name, data);
                }
            }

            meshes.emplace_back(std::move(mesh));
        }

        return meshes;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    MeshOwnedPtrArray Model::createMeshesPtr() noexcept {
        auto meshes = createMeshes();

        auto meshes_ptr = MeshOwnedPtrArray {};
        meshes_ptr.reserve(std::size(meshes));

        for (auto &mesh : meshes)
            meshes_ptr.emplace_back(std::make_unique<engine::Mesh>(std::move(mesh)));

        return meshes_ptr;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    Model::Mesh Model::doParseMesh(const tinygltf::Model &gltf_model,
                                   const tinygltf::Mesh &gltf_mesh) {
        auto mesh = Model::Mesh {};

        struct Vertex {
            core::Vector3f position;
            core::Vector3f normal;
            core::Vector2f texcoord;
            core::Vector4f tangent;
        };

        mesh.attributes = render::VertexInputAttributeDescriptionArray {
            { 0u, 0u, render::Format::Float3, offsetof(Vertex, position) },
            { 1u, 0u, render::Format::Float3, offsetof(Vertex, normal) },
            { 2u, 0u, render::Format::Float2, offsetof(Vertex, texcoord) },
            { 3u, 0u, render::Format::Float4, offsetof(Vertex, tangent) },
        };

        mesh.bindings = render::VertexBindingDescriptionArray { { 0u, sizeof(Vertex) } };

        mesh.vertex_array = VertexArray {};
        for (const auto &primitive : gltf_mesh.primitives) {
            for (const auto attrib : primitive.attributes) {
                const auto &accessor = gltf_model.accessors[attrib.second];
                if (attrib.first.compare("POSITION") == 0) {
                    mesh.vertex_array.resize(std::size(mesh.vertex_array) +
                                             accessor.count * sizeof(Vertex));
                    break;
                }
            }
        }

        const auto loadTexture = [&gltf_model, this](auto index) -> render::Texture * {
            if (index < 0) return nullptr;

            auto &gltf_texture = gltf_model.textures[index];

            if (gltf_texture.source < 0) return nullptr;

            auto &image = gltf_model.images[gltf_texture.source];

            auto hash = core::Hash64 {};
            for (const auto c : image.image) core::hash_combine(hash, c);

            auto name = fmt::format("{:x}", hash);

            if (m_texture_pool->has(name)) { return &m_texture_pool->get(name); }

            auto &texture = m_texture_pool->create(std::move(name),
                                                   m_engine->device(),
                                                   render::TextureType::T2D,
                                                   render::TextureCreateFlag::None);

            auto load_format = render::PixelFormat::RGBA8_UNorm;
            if (image.component == 1) load_format = render::PixelFormat::R8_UNorm;
            else if (image.component == 2)
                load_format = render::PixelFormat::RG8_UNorm;
            else if (image.component == 3)
                load_format = render::PixelFormat::RGB8_UNorm;

            texture.loadFromMemory({ reinterpret_cast<const std::byte *>(std::data(image.image)),
                                     std::size(image.image) },
                                   { gsl::narrow_cast<core::UInt32>(image.width),
                                     gsl::narrow_cast<core::UInt32>(image.height) },
                                   load_format);

            return &texture;
        };

        const auto addMaterialComponent = [](const auto &src_data, auto &output, auto name) {
            auto data_size = sizeof(src_data);
            auto data      = core::ByteArray {};
            data.resize(data_size);

            std::copy(reinterpret_cast<const core::Byte *>(&src_data),
                      reinterpret_cast<const core::Byte *>(&src_data) + data_size,
                      std::begin(data));

            output.emplace_back(name, std::move(data));
        };

        auto index_array   = std::vector<std::byte> {};
        auto large_indices = false;

        auto vertex_it         = 0u;
        auto last_vertex_count = 0u;
        for (const auto &primitive : gltf_mesh.primitives) {
            auto vertex_max = core::Vector3f { 0.f, 0.f, 0.f };
            auto vertex_min = core::Vector3f { 0.f, 0.f, 0.f };

            auto vertex_count = 0u;
            auto first_index  = 0u;
            auto index_count  = 0u;
            for (const auto &attrib : primitive.attributes) {
                const auto &accessor = gltf_model.accessors[attrib.second];

                const auto &buffer_view = gltf_model.bufferViews[accessor.bufferView];
                const auto &buffer      = gltf_model.buffers[buffer_view.buffer];

                const auto *input =
                    reinterpret_cast<const std::byte *>(&buffer.data[buffer_view.byteOffset]);

                auto stride = accessor.ByteStride(buffer_view);
                if (stride < 0) {
                    stride = tinygltf::GetComponentSizeInBytes(accessor.componentType) *
                             tinygltf::GetNumComponentsInType(accessor.type);
                }

                const auto count = accessor.count;

                for (auto i = 0u; i < count; ++i) {
                    const auto it =
                        reinterpret_cast<const float *>(input + accessor.byteOffset + i * stride);

                    auto &vertex = mesh.vertex_array.at<Vertex>(vertex_it + i);

                    if (attrib.first.compare("POSITION") == 0) {
                        vertex_count = count;

                        vertex_max = core::Vector3f(accessor.maxValues[0],
                                                    accessor.maxValues[1],
                                                    accessor.maxValues[2]);
                        vertex_min = core::Vector3f(accessor.minValues[0],
                                                    accessor.minValues[1],
                                                    accessor.minValues[2]);

                        vertex.position = { it[0], it[1], it[2] };
                    } else if (attrib.first.compare("NORMAL") == 0) {
                        vertex.normal = { it[0], it[1], it[2] };
                    } else if (attrib.first.compare("TANGENT") == 0) {
                        vertex.tangent = { it[0], it[1], it[2], it[4] };
                    } else if (attrib.first.compare("TEXCOORD_0") == 0) {
                        vertex.texcoord = { it[0], it[1] };
                    }
                }
            }

            if (primitive.indices >= 0) {
                const auto &accessor    = gltf_model.accessors[primitive.indices];
                const auto &buffer_view = gltf_model.bufferViews[accessor.bufferView];
                const auto &buffer      = gltf_model.buffers[buffer_view.buffer];

                index_count = accessor.count;

                const auto size = tinygltf::GetComponentSizeInBytes(accessor.componentType);

                large_indices = size == sizeof(core::UInt32);

                const auto current_size = std::size(index_array);
                index_array.resize(current_size + buffer_view.byteLength);

                const auto *data = reinterpret_cast<const std::byte *>(
                    &buffer.data[buffer_view.byteOffset + accessor.byteOffset]);
                std::copy(data,
                          data + buffer_view.byteLength,
                          std::data(index_array) + current_size);

                first_index = current_size / size;
                auto span   = core::span<std::uint16_t> { reinterpret_cast<std::uint16_t *>(
                                                            std::data(index_array) + current_size),
                                                        index_count };
                for (auto &index : span) index += last_vertex_count;
            }

            auto submesh         = Model::Mesh::Submesh {};
            submesh.vertex_count = vertex_count;
            submesh.first_index  = first_index;
            submesh.index_count  = index_count;

            if (primitive.material >= 0) {
                auto &material_instance = gltf_model.materials[primitive.material];

                const auto &gltf_color = material_instance.pbrMetallicRoughness.baseColorFactor;
                const auto color       = core::Vector4f { core::make_vec4(std::data(gltf_color)) };

                addMaterialComponent(0.f,
                                     submesh.datas,
                                     PBRMaterialInstance::AMBIANT_OCCLUSION_FACTOR_NAME);
                addMaterialComponent(color, submesh.datas, PBRMaterialInstance::ALBEDO_FACTOR_NAME);

                addMaterialComponent(static_cast<float>(
                                         material_instance.pbrMetallicRoughness.metallicFactor),
                                     submesh.datas,
                                     PBRMaterialInstance::METALLIC_FACTOR_NAME);
                addMaterialComponent(static_cast<float>(
                                         material_instance.pbrMetallicRoughness.roughnessFactor),
                                     submesh.datas,
                                     PBRMaterialInstance::ROUGHNESS_FACTOR_NAME);

                const auto &gltf_emissive = material_instance.emissiveFactor;
                const auto emissive = core::Vector4f { core::make_vec4(std::data(gltf_emissive)) };
                addMaterialComponent(emissive,
                                     submesh.datas,
                                     PBRMaterialInstance::EMISSIVE_FACTOR_NAME);

                if (auto texture =
                        loadTexture(material_instance.pbrMetallicRoughness.baseColorTexture.index);
                    texture != nullptr)
                    submesh.samplers.emplace_back(PBRMaterialInstance::ALBEDO_MAP_NAME, texture);
                if (auto texture = loadTexture(material_instance.normalTexture.index);
                    texture != nullptr)
                    submesh.samplers.emplace_back(PBRMaterialInstance::NORMAL_MAP_NAME, texture);
                if (auto texture = loadTexture(
                        material_instance.pbrMetallicRoughness.metallicRoughnessTexture.index);
                    texture != nullptr) {
                    submesh.samplers.emplace_back(PBRMaterialInstance::METALLIC_MAP_NAME, texture);
                    submesh.samplers.emplace_back(PBRMaterialInstance::ROUGHNESS_MAP_NAME, texture);
                }
                if (auto texture = loadTexture(material_instance.occlusionTexture.index);
                    texture != nullptr) {
                    submesh.samplers.emplace_back(PBRMaterialInstance::AMBIANT_OCCLUSION_MAP_NAME,
                                                  texture);
                    addMaterialComponent(1.f,
                                         submesh.datas,
                                         PBRMaterialInstance::AMBIANT_OCCLUSION_FACTOR_NAME);
                }
                if (auto texture = loadTexture(material_instance.emissiveTexture.index);
                    texture != nullptr)
                    submesh.samplers.emplace_back(PBRMaterialInstance::EMISSIVE_MAP_NAME, texture);
            }

            submesh.min = std::move(vertex_min);
            submesh.max = std::move(vertex_max);

            mesh.submeshes.emplace_back(std::move(submesh));

            vertex_it += vertex_count;
            last_vertex_count += vertex_count;
        }

        if (!std::empty(index_array)) {
            if (large_indices) {
                auto indices = LargeIndexArray {};
                indices.resize(std::size(index_array) / sizeof(core::UInt32));

                std::copy(core::ranges::begin(index_array),
                          core::ranges::end(index_array),
                          reinterpret_cast<std::byte *>(std::data(indices)));

                mesh.index_array = std::move(indices);
            } else {
                auto indices = IndexArray {};
                indices.resize(std::size(index_array) / sizeof(core::UInt16));

                std::copy(core::ranges::begin(index_array),
                          core::ranges::end(index_array),
                          reinterpret_cast<std::byte *>(std::data(indices)));

                mesh.index_array = std::move(indices);
            }
            mesh.has_indices = true;
        }
        return mesh;
    }
} // namespace storm::engine::v2
