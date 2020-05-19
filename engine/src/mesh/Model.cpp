/////////// - StormKit::core - ///////////
#include <storm/core/Hash.hpp>
#include <storm/core/Strings.hpp>

/////////// - StormKit::image - ///////////
#include <storm/image/Image.hpp>

/////////// - StormKit::log - ///////////
#include <storm/log/LogHandler.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>

#include <storm/render/pipeline/VertexInputAttributeDescription.hpp>

#include <storm/render/resource/Sampler.hpp>
#include <storm/render/resource/Texture.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/core/Vertex.hpp>

#include <storm/engine/scene/Scene.hpp>

#include <storm/engine/mesh/Model.hpp>
#include <storm/engine/mesh/StaticMesh.hpp>
#include <storm/engine/mesh/StaticSubMesh.hpp>

#include <storm/engine/material/PBRMaterial.hpp>

//#define TINYGLTF_NO_STB_IMAGE
//#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_USE_CPP14
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>

using storm::log::operator""_module;
static constexpr auto LOG_MODULE = "engine"_module;

/*bool LoadImageData(tinygltf::Image *image, const int image_id, [[maybe_unused]] std::string *err,
                   [[maybe_unused]] std::string *warn, int req_width, int req_height,
                           const unsigned char *bytes, int size, void *) {
    log::LogHandler::ilog("{}")
    return true;
}

bool WriteImageData(const std::string *basepath, const std::string *filename,
                    tinygltf::Image *image, bool embedImages, void *) {
    return true;
}*/ // TODO Implement StormKit image loading

using namespace storm;
using namespace storm::engine;

constexpr render::Format toStormKit(int type) noexcept {
    switch (type) {
        case TINYGLTF_TYPE_SCALAR: return render::Format::Float;
        case TINYGLTF_TYPE_VEC2: return render::Format::Float2;
        case TINYGLTF_TYPE_VEC3: return render::Format::Float3;
        case TINYGLTF_TYPE_VEC4: return render::Format::Float4;
    }

    return render::Format::Float;
}

////////////////////////////////////////
////////////////////////////////////////
Model::Model(Scene &scene) : m_scene { &scene } {
}

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
std::optional<StaticMesh> Model::loadStaticMeshFromFile(const std::filesystem::path &path,
                                                        Type type) {
    m_filepath = std::filesystem::absolute(path);

    auto loader = tinygltf::TinyGLTF {};
    // loader.SetImageLoader(LoadImageData, nullptr);
    // loader.SetImageWriter(WriteImageData, nullptr);

    auto model = tinygltf::Model {};

    auto error   = std::string {};
    auto warning = std::string {};

    auto res = false;

    if (type == Type::GLB) res = loader.LoadBinaryFromFile(&model, &error, &warning, path.string());
    else
        res = loader.LoadASCIIFromFile(&model, &error, &warning, path.string());
    if (!res) {
        if (!std::empty(error)) log::LogHandler::elog(LOG_MODULE, "{}", error);
        if (!std::empty(warning)) log::LogHandler::elog(LOG_MODULE, "{}", warning);

        return std::nullopt;
    }

    auto scene = model.scenes[model.defaultScene];

    auto gltf_mesh = _std::observer_ptr<const tinygltf::Mesh> {};

    auto node_queue = std::deque<std::pair<tinygltf::Node *, core::Matrixf>> {};
    for (const auto &node_id : scene.nodes) {
        auto &node = model.nodes[node_id];
        node_queue.emplace_front(&node, core::Matrixf { 1.f });
    }

    while (!std::empty(node_queue)) {
        auto [node, matrix] = node_queue.front();
        node_queue.pop_front();

        if (!std::empty(node->children)) {
            for (auto child_id : node->children) {
                auto &child = model.nodes[child_id];
                node_queue.emplace_front(&child, matrix);
            }
        }

        if (node->mesh >= 0) {
            gltf_mesh = core::makeConstObserver(model.meshes[node->mesh]);
            break;
        }
    }

    struct Vertex {
        core::Vector3f position;
        core::Vector3f normal;
        core::Vector2f texcoord;
        core::Vector4f tangent;
    };

    const auto attributes = render::TaggedVertexInputAttributeDescriptionArray {
        { "Position", { 0u, 0u, render::Format::Float3, offsetof(Vertex, position) } },
        { "Normal", { 1u, 0u, render::Format::Float3, offsetof(Vertex, normal) } },
        { "Texcoord", { 2u, 0u, render::Format::Float2, offsetof(Vertex, texcoord) } },
        { "Tangent", { 3u, 0u, render::Format::Float4, offsetof(Vertex, tangent) } },
    };

    const auto bindings = render::VertexBindingDescriptionArray { { 0u, sizeof(Vertex) } };

    auto vertex_array = VertexArray {};
    for (const auto &primitive : gltf_mesh->primitives) {
        for (const auto attrib : primitive.attributes) {
            const auto &accessor = model.accessors[attrib.second];
            if (attrib.first.compare("POSITION") == 0) {
                vertex_array.resize(std::size(vertex_array) + accessor.count * sizeof(Vertex));
                break;
            }
        }
    }

    auto &engine = m_scene->engine();

    auto mesh = engine.createStaticMesh(std::move(attributes), std::move(bindings));

    auto index_array   = std::vector<std::byte> {};
    auto large_indices = false;

    auto vertex_it         = 0u;
    auto last_vertex_count = 0u;
    for (const auto &primitive : gltf_mesh->primitives) {
        auto vertex_max = core::Vector3f { 0.f, 0.f, 0.f };
        auto vertex_min = core::Vector3f { 0.f, 0.f, 0.f };

        auto vertex_count = 0u;
        auto first_index  = 0u;
        auto index_count  = 0u;
        for (const auto &attrib : primitive.attributes) {
            const auto &accessor = model.accessors[attrib.second];

            const auto &buffer_view = model.bufferViews[accessor.bufferView];
            const auto &buffer      = model.buffers[buffer_view.buffer];

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

                auto &vertex = vertex_array.at<Vertex>(vertex_it + i);

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
            const auto &accessor    = model.accessors[primitive.indices];
            const auto &buffer_view = model.bufferViews[accessor.bufferView];
            const auto &buffer      = model.buffers[buffer_view.buffer];

            index_count = accessor.count;

            const auto size = tinygltf::GetComponentSizeInBytes(accessor.componentType);

            large_indices = size == sizeof(core::UInt32);

            const auto current_size = std::size(index_array);
            index_array.resize(current_size + buffer_view.byteLength);

            const auto *data = reinterpret_cast<const std::byte *>(
                &buffer.data[buffer_view.byteOffset + accessor.byteOffset]);
            std::copy(data, data + buffer_view.byteLength, std::data(index_array) + current_size);

            first_index = current_size / size;
            auto span   = core::span<std::uint16_t> { reinterpret_cast<std::uint16_t *>(
                                                        std::data(index_array) + current_size),
                                                    index_count };
            for (auto &index : span) index += last_vertex_count;
        }

        auto &sub_mesh = mesh.addSubmesh(vertex_count, first_index, index_count);
        sub_mesh.setBoundingBox(std::move(vertex_min), std::move(vertex_max));
        sub_mesh.setMaterialID(primitive.material);

        vertex_it += vertex_count;
        last_vertex_count += vertex_count;
    }

    mesh.setVertexArray(std::move(vertex_array));

    if (!std::empty(index_array)) {
        if (large_indices) {
            auto indices = LargeIndexArray {};
            indices.resize(std::size(index_array) / sizeof(core::UInt32));

            std::copy(core::ranges::begin(index_array),
                      core::ranges::end(index_array),
                      reinterpret_cast<std::byte *>(std::data(indices)));

            mesh.setIndexArray(std::move(indices));
        } else {
            auto indices = IndexArray {};
            indices.resize(std::size(index_array) / sizeof(core::UInt16));

            std::copy(core::ranges::begin(index_array),
                      core::ranges::end(index_array),
                      reinterpret_cast<std::byte *>(std::data(indices)));

            mesh.setIndexArray(std::move(indices));
        }
    }

    auto materials = std::vector<MaterialInstanceOwnedPtr> {};
    materials.reserve(std::size(model.materials));

    const auto loadTexture = [&model, &engine, this](auto index) -> render::Texture * {
        if (index < 0) return nullptr;

        auto &gltf_texture = model.textures[index];

        if (gltf_texture.source < 0) return nullptr;

        auto &image = model.images[gltf_texture.source];

        auto hash = core::Hash64 {};
        for (const auto c : image.image) core::hash_combine(hash, c);

        auto name = fmt::format("{:x}", hash);

        auto &texture = m_scene->texturePool().create(std::move(name),
                                                      engine.device(),
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

    auto &material = *m_scene->materialPool().get("StormKit:PBRMaterial_default");

    for (const auto &gltf_material : model.materials) {
        auto material_instance      = material.createInstancePtr();
        auto &pbr_material_instance = static_cast<PBRMaterialInstance &>(*material_instance);

        pbr_material_instance.setDebugView(PBRMaterialInstance::DebugView::None);

        const auto &color = gltf_material.pbrMetallicRoughness.baseColorFactor;
        pbr_material_instance.setAlbedoFactor({ gsl::narrow_cast<float>(color[0]),
                                                gsl::narrow_cast<float>(color[1]),
                                                gsl::narrow_cast<float>(color[2]),
                                                gsl::narrow_cast<float>(color[3]) });
        pbr_material_instance.setMetallicFactor(gltf_material.pbrMetallicRoughness.metallicFactor);
        pbr_material_instance.setRoughnessFactor(
            gltf_material.pbrMetallicRoughness.roughnessFactor);
        const auto &emissive = gltf_material.emissiveFactor;
        pbr_material_instance.setEmissiveFactor({ gsl::narrow_cast<float>(emissive[0]),
                                                  gsl::narrow_cast<float>(emissive[1]),
                                                  gsl::narrow_cast<float>(emissive[2]),
                                                  1.f });

        if (auto texture = loadTexture(gltf_material.pbrMetallicRoughness.baseColorTexture.index);
            texture != nullptr)
            pbr_material_instance.setAlbedoMap(*texture);
        if (auto texture = loadTexture(gltf_material.normalTexture.index); texture != nullptr)
            pbr_material_instance.setNormalMap(*texture);
        if (auto texture =
                loadTexture(gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index);
            texture != nullptr) {
            pbr_material_instance.setMetallicMap(*texture);
            pbr_material_instance.setRoughnessMap(*texture);
        }
        if (auto texture = loadTexture(gltf_material.occlusionTexture.index); texture != nullptr)
            pbr_material_instance.setAmbiantOcclusionMap(*texture);
        if (auto texture = loadTexture(gltf_material.emissiveTexture.index); texture != nullptr)
            pbr_material_instance.setEmissiveMap(*texture);

        materials.emplace_back(std::move(material_instance));
    }

    mesh.setMaterialInstances(std::move(materials));

    return mesh;
}
