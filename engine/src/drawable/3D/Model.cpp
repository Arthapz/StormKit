/////////// - StormKit::log - ///////////
#include <storm/log/LogHandler.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Vulkan.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/drawable/3D/Model.hpp>

#include <storm/engine/scene/PBRScene.hpp>

#include <storm/engine/material/PBRMaterial.hpp>
#include <storm/engine/material/PBRMaterialInstance.hpp>

/////////// - Tinygltf - ///////////
#define TINYGLTF_USE_CPP14
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "../../tiny_gltf.h"

using namespace storm;
using namespace storm::engine;

using storm::log::operator""_module;
static constexpr auto LOG_MODULE = "engine"_module;

bool LoadImageData(tinygltf::Image *image,
                   [[maybe_unused]] const int image_idx,
                   std::string *err,
                   [[maybe_unused]] std::string *warn,
                   [[maybe_unused]] int req_width,
                   [[maybe_unused]] int req_height,
                   const unsigned char *bytes,
                   int size,
                   void *) {
    try {
        auto _image = image::Image { { reinterpret_cast<const core::Byte *>(bytes),
                                       static_cast<core::ArraySize>(size) } };
        // if (_image.codec() == image::Image::Codec::PNG) _image = image::Image::flipY(_image);

        int pixel_type = TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE;

        image->width      = _image.extent().w;
        image->height     = _image.extent().h;
        image->component  = 4u;
        image->pixel_type = pixel_type;

        const auto size = image->width * image->height * 4u;
        image->image.resize(size);

        if (_image.channels() == 4)
            core::ranges::copy(_image.data(),
                               reinterpret_cast<core::Byte *>(std::data(image->image)));
        else {
            auto data_i   = 0u;
            auto data_ptr = _image.data();
            for (auto i = 0u; i < size; i += 4u) {
                image->image[i]     = std::to_integer<unsigned char>(data_ptr[data_i++]);
                image->image[i + 1] = std::to_integer<unsigned char>(data_ptr[data_i++]);
                image->image[i + 2] = std::to_integer<unsigned char>(data_ptr[data_i++]);
                image->image[i + 3] = 255u;
            }
        }
        return true;
    } catch (std::runtime_error &e) {
        *err = e.what();

        return false;
    }
}

constexpr render::Format toStormKit(int type) noexcept {
    switch (type) {
        case TINYGLTF_TYPE_SCALAR: return render::Format::Float;
        case TINYGLTF_TYPE_VEC2: return render::Format::Float2;
        case TINYGLTF_TYPE_VEC3: return render::Format::Float3;
        case TINYGLTF_TYPE_VEC4: return render::Format::Float4;
    }

    return render::Format::Float;
}

core::Matrixf getNodeMatrix(const tinygltf::Node &node) {
    auto translation = core::Vector3f { 0.f };
    auto rotation    = core::Matrixf { 1.f };
    auto scale       = core::Vector3f { 1.f };

    if (!std::empty(node.translation)) translation = core::make_vec3(std::data(node.translation));
    if (!std::empty(node.rotation)) {
        rotation =
            core::Matrixf { core::Quaternionf { core::make_quat(std::data(node.rotation)) } };
    }

    if (!std::empty(node.scale)) scale = core::make_vec3(std::data(node.scale));

    auto matrix = core::Matrixf { 1.f };
    if (!std::empty(node.matrix)) matrix = core::make_mat4(std::data(node.matrix));

    auto result = core::translate(core::Matrixf { 1.f }, translation) * core::Matrixf { rotation } *
                  core::scale(core::Matrixf { 1.f }, scale) * matrix;

    return result;
}

////////////////////////////////////////
////////////////////////////////////////
Model::Model(Engine &engine, TexturePool &texture_pool, MaterialPool &material_pool)
    : m_engine { &engine }, m_texture_pool { &texture_pool }, m_material_pool { &material_pool } {
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
void Model::load(const std::filesystem::path &path) {
    m_filepath = std::filesystem::absolute(path);

    auto loader = tinygltf::TinyGLTF {};
    loader.SetImageLoader(LoadImageData, nullptr);

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

    const auto name      = scene.name;
    const auto &material = *m_material_pool->get(PBRScene::DEFAULT_PBR_MATERIAL_NAME);

    m_mesh = std::make_unique<Mesh>(*m_engine, material, name);

    using NodeRef    = _std::observer_ptr<const tinygltf::Node>;
    using NodeMatrix = std::tuple<NodeRef, core::Matrixf, core::TreeNode::IndexType>;

    auto node_queue = std::deque<NodeMatrix> {};
    for (const auto &node_id : scene.nodes) {
        auto &node = model.nodes[node_id];

        auto matrix = getNodeMatrix(node);

        node_queue.emplace_front(&node, std::move(matrix), core::TreeNode::INVALID_INDEX);
    }

    while (!std::empty(node_queue)) {
        auto [node_ref, matrix, parent] = node_queue.front();
        const auto &node                = node_ref.get();
        node_queue.pop_front();

        if (node->mesh >= 0) {
            const auto &mesh = model.meshes[node->mesh];

            auto mesh_node = doParseMesh(model, mesh);
            mesh_node.setTransform(matrix);

            auto index = m_mesh->addNode(std::move(mesh_node), parent);

            parent = index;
        }

        if (!std::empty(node->children)) {
            for (auto child_id : node->children) {
                auto &child = model.nodes[child_id];

                auto child_matrix = matrix * getNodeMatrix(child);

                node_queue.emplace_front(&child, std::move(child_matrix), parent);
            }
        }
    }

    m_mesh->bake();

    m_loaded = true;
} // namespace storm::engine::v2

////////////////////////////////////////
////////////////////////////////////////
Mesh Model::createMesh() noexcept {
    STORM_EXPECTS(m_loaded);

    return m_mesh->clone();
}

////////////////////////////////////////
////////////////////////////////////////
MeshOwnedPtr Model::createMeshPtr() noexcept {
    STORM_EXPECTS(m_loaded);

    return m_mesh->clonePtr();
}

////////////////////////////////////////
////////////////////////////////////////
MeshNode Model::doParseMesh(const tinygltf::Model &model, const tinygltf::Mesh &mesh) {
    auto mesh_node = MeshNode { *m_engine, mesh.name };

    auto vertex_array = VertexArray {};
    for (const auto &primitive : mesh.primitives) {
        auto mesh_primitive = doParsePrimitive(model, primitive);

        mesh_node.addPrimitive(std::move(mesh_primitive));
    }

    return mesh_node;
}

MeshPrimitive Model::doParsePrimitive(const tinygltf::Model &model,
                                      const tinygltf::Primitive &primitive) {
    auto mesh_primitive = MeshPrimitive {};

    mesh_primitive.attributes = render::VertexInputAttributeDescriptionArray {
        { 0u, 0u, render::Format::Float3, offsetof(Vertex, position) },
        { 1u, 0u, render::Format::Float3, offsetof(Vertex, normal) },
        { 2u, 0u, render::Format::Float2, offsetof(Vertex, texcoord) },
        { 3u, 0u, render::Format::Float4, offsetof(Vertex, tangent) },
        { 4u, 0u, render::Format::UInt4, offsetof(Vertex, join_id) },
        { 5u, 0u, render::Format::Float4, offsetof(Vertex, weight) },
    };

    mesh_primitive.bindings = render::VertexBindingDescriptionArray { { 0u, sizeof(Vertex) } };

    auto vertex_max = core::Vector3f { 0.f, 0.f, 0.f };
    auto vertex_min = core::Vector3f { 0.f, 0.f, 0.f };

    for (const auto &attribute : primitive.attributes) {
        const auto &accessor    = model.accessors[attribute.second];
        const auto &buffer_view = model.bufferViews[accessor.bufferView];
        const auto &buffer      = model.buffers[buffer_view.buffer];
        const auto *buffer_data =
            reinterpret_cast<const std::byte *>(&buffer.data[buffer_view.byteOffset]);

        auto stride = accessor.ByteStride(buffer_view);
        if (stride < 0) {
            stride = tinygltf::GetComponentSizeInBytes(accessor.componentType) *
                     tinygltf::GetNumComponentsInType(accessor.type);
        }

        const auto count = accessor.count;

        if (std::empty(mesh_primitive.vertices))
            mesh_primitive.vertices.resize(count * sizeof(Vertex));
        mesh_primitive.vertex_count = count;

        for (auto i = 0u; i < count; ++i) {
            const auto it =
                reinterpret_cast<const float *>(buffer_data + accessor.byteOffset + i * stride);

            auto &vertex = mesh_primitive.vertices.at<Vertex>(i);

            if (attribute.first.compare("POSITION") == 0) {
                vertex_max = core::Vector3f(accessor.maxValues[0],
                                            accessor.maxValues[1],
                                            accessor.maxValues[2]);
                vertex_min = core::Vector3f(accessor.minValues[0],
                                            accessor.minValues[1],
                                            accessor.minValues[2]);

                vertex.position = core::make_vec3(it);
            } else if (attribute.first.compare("NORMAL") == 0)
                vertex.normal = core::make_vec3(it);
            else if (attribute.first.compare("TANGENT") == 0)
                vertex.tangent = core::make_vec4(it);
            else if (attribute.first.compare("TEXCOORD_0") == 0)
                vertex.texcoord = core::make_vec2(it);
            else if (attribute.first.compare("JOINTS_0") == 0)
                vertex.join_id = core::make_vec4(it);
            else if (attribute.first.compare("WEIGHTS_0") == 0)
                vertex.join_id = core::make_vec4(it);
        }
    }

    mesh_primitive.bounding_box.min    = std::move(vertex_min);
    mesh_primitive.bounding_box.max    = std::move(vertex_max);
    mesh_primitive.bounding_box.extent = core::Extentf {
        .w = mesh_primitive.bounding_box.max.x - mesh_primitive.bounding_box.min.x,
        .h = mesh_primitive.bounding_box.max.y - mesh_primitive.bounding_box.min.y,
        .d = mesh_primitive.bounding_box.max.z - mesh_primitive.bounding_box.min.z,
    };

    if (primitive.indices >= 0) {
        const auto &accessor    = model.accessors[primitive.indices];
        const auto &buffer_view = model.bufferViews[accessor.bufferView];
        const auto &buffer      = model.buffers[buffer_view.buffer];
        const auto *buffer_data =
            reinterpret_cast<const std::byte *>(&buffer.data[buffer_view.byteOffset]);

        mesh_primitive.index_count = accessor.count;

        const auto size = tinygltf::GetComponentSizeInBytes(accessor.componentType);

        auto large_indices = size == sizeof(core::UInt32);

        const auto fill_indices = [&buffer_data, &mesh_primitive, &size](auto &indices) {
            using IndexType = typename std::remove_reference_t<decltype(indices)>::value_type;

            for (auto i = 0u; i < mesh_primitive.index_count; ++i) {
                const auto it = &buffer_data[i * size];

                auto index = IndexType {};
                for (auto j = 0; j < size; ++j) { index |= static_cast<IndexType>(it[j]) << j; }

                indices[i] = index;
            }
        };

        if (large_indices) {
            auto indices = LargeIndexArray {};
            indices.resize(mesh_primitive.index_count);

            fill_indices(indices);
            mesh_primitive.indices       = std::move(indices);
            mesh_primitive.large_indices = true;
        } else {
            auto indices = IndexArray {};
            indices.resize(mesh_primitive.index_count);

            fill_indices(indices);
            mesh_primitive.indices       = std::move(indices);
            mesh_primitive.large_indices = false;
        }
    }

    if (primitive.material >= 0) {
        const auto &material = model.materials[primitive.material];

        auto material_instance = doParseMaterialInstance(model, material);

        mesh_primitive.material_instance = std::move(material_instance);
    } else {
        mesh_primitive.material_instance = m_mesh->material().createInstancePtr();
    }

    return mesh_primitive;
}

////////////////////////////////////////
////////////////////////////////////////
MaterialInstanceOwnedPtr Model::doParseMaterialInstance(const tinygltf::Model &model,
                                                        const tinygltf::Material &material) {
    const auto loadTexture = [&model, this](auto index) -> render::Texture * {
        if (index < 0) return nullptr;

        auto &gltf_texture = model.textures[index];

        if (gltf_texture.source < 0) return nullptr;

        auto &image = model.images[gltf_texture.source];

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

        const auto extent = core::Extentu { .w = static_cast<core::UInt32>(image.width),
                                            .h = static_cast<core::UInt32>(image.height) };
        texture.loadFromMemory({ reinterpret_cast<const std::byte *>(std::data(image.image)),
                                 std::size(image.image) },
                               extent,
                               load_format,
                               render::Texture::LoadOperation {
                                   .generate_mip_map = true,
                                   .storage_format   = render::PixelFormat::RGBA8_UNorm,
                                   .samples          = render::SampleCountFlag::C1_BIT,
                                   .mip_levels       = render::computeMipLevel(extent),
                                   .usage            = render::TextureUsage::Sampled |
                                            render::TextureUsage::Transfert_Dst |
                                            render::TextureUsage::Transfert_Src });
        m_engine->device().setObjectName(texture, name);

        return &texture;
    };

    const auto addMaterialComponent = [](const auto &src_data, auto &material_instance, auto name) {
        auto data_size = sizeof(src_data);
        auto data      = core::ByteArray {};
        data.resize(data_size);

        std::copy(reinterpret_cast<const core::Byte *>(&src_data),
                  reinterpret_cast<const core::Byte *>(&src_data) + data_size,
                  std::begin(data));

        material_instance.setRawDataValue(std::move(name), data);
    };

    auto material_instance = m_mesh->material().createInstancePtr();

    const auto &gltf_color = material.pbrMetallicRoughness.baseColorFactor;
    const auto color       = core::Vector4f { core::make_vec4(std::data(gltf_color)) };

    addMaterialComponent(0.f,
                         *material_instance,
                         PBRMaterialInstance::AMBIANT_OCCLUSION_FACTOR_NAME);
    addMaterialComponent(color, *material_instance, PBRMaterialInstance::ALBEDO_FACTOR_NAME);
    addMaterialComponent(static_cast<float>(material.pbrMetallicRoughness.metallicFactor),
                         *material_instance,
                         PBRMaterialInstance::METALLIC_FACTOR_NAME);
    addMaterialComponent(static_cast<float>(material.pbrMetallicRoughness.roughnessFactor),
                         *material_instance,
                         PBRMaterialInstance::ROUGHNESS_FACTOR_NAME);

    const auto &gltf_emissive = material.emissiveFactor;
    const auto emissive       = core::Vector4f { core::make_vec4(std::data(gltf_emissive)) };
    addMaterialComponent(emissive, *material_instance, PBRMaterialInstance::EMISSIVE_FACTOR_NAME);

    if (auto texture = loadTexture(material.pbrMetallicRoughness.baseColorTexture.index);
        texture != nullptr) {
        const auto settings =
            render::Sampler::Settings { .enable_anisotropy = true,
                                        .max_anisotropy    = m_engine->maxAnisotropy(),
                                        .compare_operation = render::CompareOperation::Never,
                                        .max_lod = static_cast<float>(texture->mipLevels()) };

        material_instance->setSampledTexture(PBRMaterialInstance::ALBEDO_MAP_NAME,
                                             *texture,
                                             render::TextureViewType::T2D,
                                             {},
                                             std::move(settings));
    }

    if (auto texture = loadTexture(material.normalTexture.index); texture != nullptr) {
        const auto settings =
            render::Sampler::Settings { .enable_anisotropy = true,
                                        .max_anisotropy    = m_engine->maxAnisotropy(),
                                        .compare_operation = render::CompareOperation::Never,
                                        .max_lod = static_cast<float>(texture->mipLevels()) };

        material_instance->setSampledTexture(PBRMaterialInstance::NORMAL_MAP_NAME,
                                             *texture,
                                             render::TextureViewType::T2D,
                                             {},
                                             std::move(settings));
    }

    if (auto texture = loadTexture(material.pbrMetallicRoughness.metallicRoughnessTexture.index);
        texture != nullptr) {
        const auto settings =
            render::Sampler::Settings { .enable_anisotropy = true,
                                        .max_anisotropy    = m_engine->maxAnisotropy(),
                                        .compare_operation = render::CompareOperation::Never,
                                        .max_lod = static_cast<float>(texture->mipLevels()) };

        material_instance->setSampledTexture(PBRMaterialInstance::METALLIC_MAP_NAME,
                                             *texture,
                                             render::TextureViewType::T2D,
                                             {},
                                             settings);

        material_instance->setSampledTexture(PBRMaterialInstance::ROUGHNESS_MAP_NAME,
                                             *texture,
                                             render::TextureViewType::T2D,
                                             {},
                                             std::move(settings));
    }

    if (auto texture = loadTexture(material.occlusionTexture.index); texture != nullptr) {
        const auto settings =
            render::Sampler::Settings { .enable_anisotropy = true,
                                        .max_anisotropy    = m_engine->maxAnisotropy(),
                                        .compare_operation = render::CompareOperation::Never,
                                        .max_lod = static_cast<float>(texture->mipLevels()) };

        material_instance->setSampledTexture(PBRMaterialInstance::AMBIANT_OCCLUSION_MAP_NAME,
                                             *texture,
                                             render::TextureViewType::T2D,
                                             {},
                                             std::move(settings));
        addMaterialComponent(1.f,
                             *material_instance,
                             PBRMaterialInstance::AMBIANT_OCCLUSION_FACTOR_NAME);
    }

    if (auto texture = loadTexture(material.emissiveTexture.index); texture != nullptr) {
        const auto settings =
            render::Sampler::Settings { .enable_anisotropy = true,
                                        .max_anisotropy    = m_engine->maxAnisotropy(),
                                        .compare_operation = render::CompareOperation::Never,
                                        .max_lod = static_cast<float>(texture->mipLevels()) };

        material_instance->setSampledTexture(PBRMaterialInstance::EMISSIVE_MAP_NAME,
                                             *texture,
                                             render::TextureViewType::T2D,
                                             {},
                                             std::move(settings));
    }

    if (material.doubleSided) material_instance->setCullMode(render::CullMode::None);

    return material_instance;
}
