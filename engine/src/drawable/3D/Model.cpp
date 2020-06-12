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
    auto _image = image::Image { core::toConstSpan<core::Byte>(bytes, size) }.toFormat(
        image::Image::Format::RGBA);
    // if (_image.codec() == image::Image::Codec::PNG) _image = image::Image::flipY(_image);

    int pixel_type = TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE;

    image->width      = _image.extent().w;
    image->height     = _image.extent().h;
    image->component  = 4u;
    image->pixel_type = pixel_type;

    const auto _size = image->width * image->height * 4u;
    image->image.resize(_size);

    core::ranges::copy(_image.data(), reinterpret_cast<core::Byte *>(std::data(image->image)));

    return true;
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

    for (const auto &skin : model.skins) {
        auto mesh_skin = doParseSkin(model, skin);

        m_mesh->addSkin(std::move(mesh_skin));
    }

    for (const auto &animation : model.animations) {
        auto mesh_animation = doParseAnimation(model, animation);

        m_mesh->addAnimation(std::move(mesh_animation));
    }

    auto parents = core::HashMap<Mesh::Node::ID, Mesh::Node::ID> {};

    auto nodes = std::deque<Mesh::Node::ID> {};
    for (const auto node_id : scene.nodes) {
        nodes.emplace_back(node_id);

        parents[node_id] = Mesh::Node::INVALID;
    }

    while (!std::empty(nodes)) {
        const auto node_id = nodes.front();
        nodes.pop_front();

        const auto &node = model.nodes[node_id];

        for (const auto child_id : node.children) {
            parents[child_id] = node_id;

            nodes.emplace_back(child_id);
        }
    }

    auto i = 0;
    for (const auto &node : model.nodes) {
        auto mesh_node   = Mesh::Node {};
        mesh_node.parent = parents[i++];

        if (!std::empty(node.translation))
            mesh_node.translation = core::make_vec3(std::data(node.translation));
        if (!std::empty(node.rotation))
            mesh_node.rotation = core::Quaternionf { core::make_quat(std::data(node.rotation)) };
        if (!std::empty(node.scale)) mesh_node.scale = core::make_vec3(std::data(node.scale));
        if (!std::empty(node.matrix)) mesh_node.matrix = core::make_mat4(std::data(node.matrix));

        if (node.mesh >= 0) {
            const auto &mesh = model.meshes[node.mesh];

            mesh_node.submesh = doParseMesh(model, mesh);
        }

        if (node.skin >= 0) mesh_node.skin = node.skin;

        for (const auto child_id : node.children) mesh_node.childs.emplace_back(child_id);

        m_mesh->addNode(std::move(mesh_node));
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
SubMesh Model::doParseMesh(const tinygltf::Model &model, const tinygltf::Mesh &mesh) {
    auto mesh_node = SubMesh { *m_engine, mesh.name };

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
        { 4u, 0u, render::Format::UInt4, offsetof(Vertex, joint_id) },
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

        if (count > std::size(mesh_primitive.vertices)) {
            mesh_primitive.vertices.resize(count * sizeof(Vertex));
            mesh_primitive.vertex_count = count;
        }

        for (auto i = 0u; i < count; ++i) {
            const auto it = buffer_data + accessor.byteOffset + i * stride;

            auto &vertex = mesh_primitive.vertices.at<Vertex>(i);

            if (attribute.first.compare("POSITION") == 0) {
                vertex_max = core::make_vec3(std::data(accessor.maxValues));
                vertex_min = core::make_vec3(std::data(accessor.minValues));

                vertex.position = core::make_vec3(reinterpret_cast<const float *>(it));
            } else if (attribute.first.compare("NORMAL") == 0)
                vertex.normal = core::make_vec3(reinterpret_cast<const float *>(it));
            else if (attribute.first.compare("TANGENT") == 0)
                vertex.tangent = core::make_vec4(reinterpret_cast<const float *>(it));
            else if (attribute.first.compare("TEXCOORD_0") == 0) {
                if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                    vertex.texcoord = core::make_vec2<float>(reinterpret_cast<const float *>(it));
                } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
                    vertex.texcoord =
                        core::make_vec2<core::UInt8>(reinterpret_cast<const core::UInt8 *>(it));
                } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                    vertex.texcoord =
                        core::make_vec2<core::UInt16>(reinterpret_cast<const core::UInt16 *>(it));
                }
            } else if (attribute.first.compare("JOINTS_0") == 0) {
                if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
                    vertex.joint_id =
                        core::make_vec4<core::UInt8>(reinterpret_cast<const core::UInt8 *>(it));
                } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                    vertex.joint_id =
                        core::make_vec4<core::UInt16>(reinterpret_cast<const core::UInt16 *>(it));
                }
            } else if (attribute.first.compare("WEIGHTS_0") == 0) {
                if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                    vertex.weight = core::make_vec4<float>(reinterpret_cast<const float *>(it));
                } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
                    vertex.weight =
                        core::make_vec4<core::UInt8>(reinterpret_cast<const core::UInt8 *>(it));
                } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                    vertex.weight =
                        core::make_vec4<core::UInt16>(reinterpret_cast<const core::UInt16 *>(it));
                }
            }
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
        const auto *buffer_data = reinterpret_cast<const std::byte *>(
            &buffer.data[accessor.byteOffset + buffer_view.byteOffset]);

        mesh_primitive.index_count = accessor.count;

        const auto size = tinygltf::GetComponentSizeInBytes(accessor.componentType);

        auto large_indices = size == sizeof(core::UInt32);

        const auto fill_indices = [&buffer_data, &mesh_primitive, &size](auto &indices) {
            if (size == 1) {
                const auto data = reinterpret_cast<const core::UInt8 *>(buffer_data);
                for (auto i = 0u; i < mesh_primitive.index_count; ++i) { indices[i] = data[i]; }
            } else if (size == 2) {
                const auto data = reinterpret_cast<const core::UInt16 *>(buffer_data);
                for (auto i = 0u; i < mesh_primitive.index_count; ++i) { indices[i] = data[i]; }
            } else if (size == 4) {
                const auto data = reinterpret_cast<const core::UInt32 *>(buffer_data);
                for (auto i = 0u; i < mesh_primitive.index_count; ++i) { indices[i] = data[i]; }
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

        const auto extent = core::Extentu { static_cast<core::UInt32>(image.width),
                                            static_cast<core::UInt32>(image.height) };
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

////////////////////////////////////////
////////////////////////////////////////
Mesh::Skin Model::doParseSkin(const tinygltf::Model &model, const tinygltf::Skin &skin) {
    auto mesh_skin = Mesh::Skin {};
    mesh_skin.name = skin.name;

    for (const auto joint_id : skin.joints) mesh_skin.joints.emplace_back(joint_id);

    if (skin.inverseBindMatrices >= 0) {
        const auto &accessor    = model.accessors[skin.inverseBindMatrices];
        const auto &buffer_view = model.bufferViews[accessor.bufferView];
        const auto &buffer      = model.buffers[buffer_view.buffer];

        const auto offset = accessor.byteOffset + buffer_view.byteOffset;

        mesh_skin.inverse_bind_matrices.resize(accessor.count);
        std::memcpy(std::data(mesh_skin.inverse_bind_matrices),
                    &buffer.data[offset],
                    accessor.count * sizeof(core::Matrixf));
    }

    return mesh_skin;
}

////////////////////////////////////////
////////////////////////////////////////
Mesh::Animation Model::doParseAnimation(const tinygltf::Model &model,
                                        const tinygltf::Animation &animation) {
    auto mesh_animation = Mesh::Animation {};
    mesh_animation.samplers.reserve(std::size(animation.samplers));
    mesh_animation.channels.reserve(std::size(animation.channels));
    mesh_animation.name = animation.name;

    for (auto &sampler : animation.samplers) {
        auto &mesh_sampler = mesh_animation.samplers.emplace_back();

        if (sampler.interpolation == "LINEAR")
            mesh_sampler.interpolation = Mesh::InterpolationType::Linear;
        else if (sampler.interpolation == "STEP")
            mesh_sampler.interpolation = Mesh::InterpolationType::Step;
        else if (sampler.interpolation == "CUBICSPLINE")
            mesh_sampler.interpolation = Mesh::InterpolationType::Cubic_Spline;

        {
            const auto &accessor    = model.accessors[sampler.input];
            const auto &buffer_view = model.bufferViews[accessor.bufferView];
            const auto &buffer      = model.buffers[buffer_view.buffer];

            const auto offset = accessor.byteOffset + buffer_view.byteOffset;

            const auto data = reinterpret_cast<const float *>(&buffer.data[offset]);

            mesh_sampler.inputs.resize(accessor.count);
            core::ranges::copy(data,
                               data + accessor.count,
                               core::ranges::begin(mesh_sampler.inputs));

            for (auto input : mesh_sampler.inputs) {
                const auto input_time = Mesh::Animation::Seconds { input };
                if (input_time < mesh_animation.start) mesh_animation.start = input_time;
                if (input_time > mesh_animation.end) mesh_animation.end = input_time;
            }
        }

        {
            const auto &accessor    = model.accessors[sampler.output];
            const auto &buffer_view = model.bufferViews[accessor.bufferView];
            const auto &buffer      = model.buffers[buffer_view.buffer];

            const auto offset = accessor.byteOffset + buffer_view.byteOffset;

            const auto data = reinterpret_cast<const float *>(&buffer.data[offset]);

            mesh_sampler.outputs.reserve(accessor.count);
            switch (accessor.type) {
                case TINYGLTF_TYPE_VEC3:
                    for (auto i = 0u; i < accessor.count; ++i) {
                        mesh_sampler.outputs.emplace_back(
                            core::make_vec4(core::make_vec3(&data[i * 3])));
                    }
                    break;
                case TINYGLTF_TYPE_VEC4:
                    for (auto i = 0u; i < accessor.count; ++i) {
                        mesh_sampler.outputs.emplace_back(core::make_vec4(&data[i * 4]));
                    }
                    break;
            }
        }
    }

    for (const auto &channel : animation.channels) {
        auto &mesh_channel = mesh_animation.channels.emplace_back();

        mesh_channel.node = channel.target_node;
        if (channel.target_path == "translation") mesh_channel.path = Mesh::Path::Translation;
        else if (channel.target_path == "rotation")
            mesh_channel.path = Mesh::Path::Rotation;
        else if (channel.target_path == "scale")
            mesh_channel.path = Mesh::Path::Scale;
        else if (channel.target_path == "weight")
            mesh_channel.path = Mesh::Path::Weights;

        mesh_channel.sampler = channel.sampler;
    }

    return mesh_animation;
}
