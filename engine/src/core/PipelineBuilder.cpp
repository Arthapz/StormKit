/////////// - StormKit::render - ///////////
#include <storm/render/core/Surface.hpp>

#include <storm/render/resource/Shader.hpp>

#include <storm/render/pipeline/GraphicsPipelineState.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/core/PipelineBuilder.hpp>

/////////// - ShaderWriter - ///////////
#undef CASE

#include <CompilerGlsl/compileGlsl.hpp>
#include <CompilerSpirV/compileSpirV.hpp>
#include <ShaderWriter/Source.hpp>

using namespace storm;
using namespace storm::engine;

/////////////////////////////////////
/////////////////////////////////////
PipelineBuilder::PipelineBuilder(Engine &engine) : m_engine { &engine } {
    createCommonLayouts();
    createPBRLayouts();
}

/////////////////////////////////////
/////////////////////////////////////
PipelineBuilder::~PipelineBuilder() = default;

/////////////////////////////////////
/////////////////////////////////////
PipelineBuilder::PipelineBuilder(PipelineBuilder &&) = default;

/////////////////////////////////////
/////////////////////////////////////
PipelineBuilder &PipelineBuilder::operator=(PipelineBuilder &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void PipelineBuilder::createPBRPipeline(const StaticMesh &mesh,
                                        Material &material,
                                        bool msaa,
                                        std::optional<render::SampleCountFlag> samples) {
    auto &shader_pool = m_engine->shaderPool();

    const auto surface_extent  = m_engine->surface().extent();
    const auto surface_extentf = surface_extent.convertTo<storm::core::Extentf>();

    const auto viewports = std::vector { render::Viewport { .position = { 0.f, 0.f },
                                                            .extent   = surface_extentf,
                                                            .depth    = { 0.f, 1.f } } };
    const auto scissors =
        std::vector { render::Scissor { .offset = { 0, 0 }, .extent = surface_extent } };

    auto bindings                 = mesh.vertexBindings();
    auto attributes               = mesh.vertexAttributes();
    const auto &tagged_attributes = mesh.taggedVertexAttributes();

    auto bindings_hash = core::Hash64 { 0u };
    core::hash_combine(bindings_hash, bindings);
    auto attributes_hash = core::Hash64 { 0u };
    core::hash_combine(attributes_hash, attributes);

    auto material_interface_map = core::Hash64 { 0u };
    core::hash_combine(material_interface_map, material.hasBaseColorMap());
    core::hash_combine(material_interface_map, material.hasNormalMap());
    core::hash_combine(material_interface_map, material.hasMetallicRoughnessMap());
    core::hash_combine(material_interface_map, material.hasAmbiantOcclusionMap());
    core::hash_combine(material_interface_map, material.hasEmissiveMap());

    const auto vertex_shader_name =
        fmt::format("StormKit:PBRShader:Vertex:{}:{}", attributes_hash, bindings_hash);
    const auto fragment_shader_name =
        fmt::format("StormKit:PBRShader:Fragment:{}", material_interface_map);

    if (!shader_pool.has(vertex_shader_name))
        createPBRVertexShader(vertex_shader_name, tagged_attributes);
    if (!shader_pool.has(fragment_shader_name))
        createPBRFragmentShader(fragment_shader_name, material);

    const auto shaders = core::makeConstObservers(shader_pool.get(vertex_shader_name),
                                                  shader_pool.get(fragment_shader_name));

    auto attachments       = std::vector { render::GraphicsPipelineColorBlendAttachmentState {} };
    auto multisample_state = render::GraphicsPipelineMultiSampleState {};

    if (msaa) {
        multisample_state.sample_shading_enable = false;
        multisample_state.rasterization_samples = samples.value_or(m_engine->maxSampleCount());
    }

    auto pipeline = render::GraphicsPipelineState {
        .viewport_state    = { .viewports = std::move(viewports), .scissors = std::move(scissors) },
        .multisample_state = std::move(multisample_state),
        .color_blend_state = { .attachments = std::move(attachments) },
        .shader_state      = { .shaders = std::move(shaders) },
        .vertex_input_state  = { .binding_descriptions         = bindings,
                                .input_attribute_descriptions = attributes },
        .depth_stencil_state = { .depth_test_enable  = true,
                                 .depth_write_enable = true,
                                 .depth_compare_op   = render::CompareOperation::Less },
        .layout              = m_pbr_pipeline_layout
    };

    material.setGraphicsPipelineState(std::move(pipeline));
}

void PipelineBuilder::createCommonLayouts() {
    auto &device = m_engine->device();

    m_camera_layout = device.createDescriptorSetLayoutPtr();
    m_camera_layout->addBinding({ 0,
                                  render::DescriptorType::Uniform_Buffer_Dynamic,
                                  render::ShaderStage::Vertex | render::ShaderStage::Fragment,
                                  1 });
    m_camera_layout->bake();

    m_transform_layout = device.createDescriptorSetLayoutPtr();
    m_transform_layout->addBinding(
        { 0, render::DescriptorType::Uniform_Buffer_Dynamic, render::ShaderStage::Vertex, 1 });
    m_transform_layout->bake();
}

void PipelineBuilder::createPBRLayouts() {
    auto &device = m_engine->device();

    m_pbr_material_layout = device.createDescriptorSetLayoutPtr();
    m_pbr_material_layout->addBinding(
        { 0, render::DescriptorType::Combined_Texture_Sampler, render::ShaderStage::Fragment, 1 });
    m_pbr_material_layout->addBinding(
        { 1, render::DescriptorType::Combined_Texture_Sampler, render::ShaderStage::Fragment, 1 });
    m_pbr_material_layout->addBinding(
        { 2, render::DescriptorType::Combined_Texture_Sampler, render::ShaderStage::Fragment, 1 });
    m_pbr_material_layout->addBinding(
        { 3, render::DescriptorType::Combined_Texture_Sampler, render::ShaderStage::Fragment, 1 });
    m_pbr_material_layout->addBinding(
        { 4, render::DescriptorType::Combined_Texture_Sampler, render::ShaderStage::Fragment, 1 });
    m_pbr_material_layout->bake();

    m_pbr_pipeline_layout.descriptor_set_layouts = { core::makeConstObserver(m_camera_layout),
                                                     core::makeConstObserver(m_transform_layout),
                                                     core::makeConstObserver(
                                                         m_pbr_material_layout) };

    m_pbr_pipeline_layout.push_constant_ranges = {
        { render::ShaderStage::Fragment, 0u, sizeof(Material::Data) },
    };
}

void PipelineBuilder::createPBRVertexShader(
    std::string_view name,
    const render::TaggedVertexInputAttributeDescriptionArray &attributes) {
    auto &shader_pool = m_engine->shaderPool();

    auto writer = sdw::VertexWriter {};

    auto position =
        writer.declInput<sdw::Vec3>("vertex_position", attributes.at("Position").location);
    auto normal = writer.declInput<sdw::Vec3>("vertex_normal", attributes.at("Normal").location);
    auto texcoord =
        writer.declInput<sdw::Vec2>("vertex_texcoord", attributes.at("Texcoord").location);
    auto tangent = writer.declInput<sdw::Vec4>("tangent", attributes.at("Tangent").location);

    auto out_position = writer.declOutput<sdw::Vec3>("out_vertex_position", 0u);
    auto out_normal   = writer.declOutput<sdw::Vec3>("out_vertex_normal", 1u);
    auto out_texcoord = writer.declOutput<sdw::Vec2>("out_vertex_texcoord", 2u);
    auto out_tangent  = writer.declOutput<sdw::Vec4>("out_vertex_tangent", 3u);

    auto camera = sdw::Ubo { writer, "camera", 0, 0 };
    camera.declMember<sdw::Vec4>("camera_position");
    camera.declMember<sdw::Mat4>("camera_projection");
    camera.declMember<sdw::Mat4>("camera_view");
    camera.end();

    auto transform = sdw::Ubo { writer, "transform", 0, 1 };
    transform.declMember<sdw::Mat4>("transform_model");
    transform.declMember<sdw::Mat4>("transform_inverted_model");
    transform.end();

    auto out = writer.getOut();

    writer.implementFunction<void>("main", [&]() {
        using namespace sdw;

        auto model_space_position =
            writer.declLocale("model_space_position",
                              transform.getMember<Mat4>("transform_model") * vec4(position, 1.f));

        out_position = model_space_position.xyz() / model_space_position.w();
        out_normal   = normalize(
            transpose(mat3(transform.getMember<Mat4>("transform_inverted_model"))) * normal);
        out_texcoord = texcoord;
        out_tangent  = tangent;

        out.vtx.position = camera.getMember<Mat4>("camera_projection") *
                           camera.getMember<Mat4>("camera_view") * vec4(out_position, 1.f);
    });

    auto spirv = spirv::serialiseSpirv(writer.getShader());

    shader_pool.create(std::string { name },
                       spirv,
                       render::ShaderStage::Vertex,
                       m_engine->device());
}

void PipelineBuilder::createPBRFragmentShader(std::string_view name,
                                              const Material &mesh_material) {
    auto &shader_pool = m_engine->shaderPool();
    auto writer       = sdw::FragmentWriter {};

    auto in_position = writer.declInput<sdw::Vec3>("in_vertex_position", 0u);
    auto in_normal   = writer.declInput<sdw::Vec3>("in_vertex_normal", 1u);
    auto in_texcoord = writer.declInput<sdw::Vec2>("in_vertex_texcoord", 2u);
    auto in_tangent  = writer.declInput<sdw::Vec4>("in_vertex_tangent", 3u);

    auto frag_color = writer.declOutput<sdw::Vec4>("frag_color", 0u);

    auto camera = sdw::Ubo { writer, "camera", 0, 0 };
    camera.declMember<sdw::Vec4>("camera_position");
    camera.declMember<sdw::Mat4>("camera_projection");
    camera.declMember<sdw::Mat4>("camera_view");
    camera.end();

    auto material = sdw::Pcb { writer, "material", ast::type::MemoryLayout::eStd140 };
    material.declMember<sdw::Vec4>("material_base_color_factor");
    material.declMember<sdw::Vec4>("material_emissive_factor");
    material.declMember<sdw::Float>("material_metallic_factor");
    material.declMember<sdw::Float>("material_roughness_factor");
    material.declMember<sdw::Float>("material_ambiant_occlusion_factor");
    material.declMember<sdw::Float>("material_PAD0");
    material.end();

    auto base_color_sampler =
        writer.declSampledImage<FImg2DRgba32>("material_base_color_sampler", 0, 2);
    auto normal_sampler = writer.declSampledImage<FImg2DRgba32>("material_normal_sampler", 1, 2);
    auto metallic_roughness_sampler =
        writer.declSampledImage<FImg2DRgba32>("material_metallic_roughness_sampler", 2, 2);
    auto ambiant_occlusion_sampler =
        writer.declSampledImage<FImg2DRgba32>("material_ambiant_occlusion_sampler", 3, 2);
    auto emissive_sampler =
        writer.declSampledImage<FImg2DRgba32>("material_emissive_sampler", 4, 2);

    auto out = writer.getOut();

    auto getEmissiveColor = writer.implementFunction<sdw::Vec4>("getEmissiveColor", [&]() {
        using namespace sdw;
        if (mesh_material.hasEmissiveMap())
            writer.returnStmt(sdw::texture(emissive_sampler, in_texcoord) *
                              material.getMember<Vec4>("material_emissive_factor"));
        else
            writer.returnStmt(vec4(0._f));
    });

    /*auto getNormal = writer.implementFunction<sdw::Vec3>("getNormal", [&]() {
        if (mesh_material.hasNormalMap()) {
            auto tangent_normal =
                writer.declLocale("tangent_normal",
                                  sdw::texture(normal_sampler, in_texcoord).xyz() * 2.f - 1.f);

            auto N = writer.declLocale("N", sdw::normalize(in_normal));
            auto T = writer.declLocale("T", sdw::normalize(in_tangent.xyz()));
            auto B = writer.declLocale("B", -sdw::normalize(sdw::cross(N, T)));

            auto TBN = writer.declLocale<sdw::Mat3>("TBN", sdw::mat3(T, B, N));

            writer.returnStmt(sdw::normalize(TBN * tangent_normal));
        } else
            writer.returnStmt(in_normal);
    });*/

    writer.implementFunction<void>("main", [&]() {
        using namespace sdw;
        auto light_color     = writer.declConstant("light_color", vec3(1._f));
        auto light_direction = writer.declConstant("light_direction", vec3(0._f, 0.f, -10._f));

        auto base_color =
            writer.declLocale("base_color", material.getMember<Vec4>("material_base_color_factor"));
        auto metallic =
            writer.declLocale("metallic", material.getMember<Float>("material_metallic_factor"));
        auto roughness =
            writer.declLocale("roughness", material.getMember<Float>("material_roughness_factor"));

        if (mesh_material.hasBaseColorMap()) {
            base_color = texture(base_color_sampler, in_texcoord);

            auto lin_out =
                writer.declLocale("base_color_lin_out", pow(base_color.xyz(), vec3(2.2_f)));
            base_color = vec4(lin_out, base_color.a());
            base_color *= material.getMember<Vec4>("material_base_color_factor");
        }

        if (mesh_material.hasMetallicRoughnessMap()) {
            auto sample = writer.declLocale("metallic_roughness_sample",
                                            texture(metallic_roughness_sampler, in_texcoord));
            metallic *= sample.b();
            roughness *= sample.g();
        }

        auto diffuse =
            writer.declLocale("diffuse_color", base_color.rgb() * (vec3(1._f) - vec3(0.04_f)));
        diffuse *= (1.f - metallic);

        auto N = writer.declLocale("N", normalize(in_normal));
        if (mesh_material.hasNormalMap()) {
            auto tangent_normal =
                writer.declLocale("tangent_normal",
                                  texture(normal_sampler, in_texcoord).xyz() * 2.f - 1.f);

            // auto N = writer.declLocale("N", sdw::normalize(in_normal));
            auto T = writer.declLocale("T", normalize(in_tangent.xyz()));
            auto B = writer.declLocale("B", -normalize(cross(N, T)));

            auto TBN = writer.declLocale("TBN", mat3(T, B, N));

            N = normalize(TBN * tangent_normal);
        }

        auto emissive_color = getEmissiveColor();

        auto alpha_roughness = pow(roughness, 2._f);

        auto specular_color =
            writer.declLocale("specular_color",
                              mix(vec3(0.04_f), base_color.rgb(), vec3(metallic)));

        auto reflectance =
            writer.declLocale("reflectance",
                              max(max(specular_color.r(), specular_color.g()), specular_color.b()));

        auto reflectance90 =
            writer.declLocale("reflectance90", clamp(reflectance * 25.f, 0._f, 1._f));
        auto specular_env_r90 = writer.declLocale("specular_env_r90", vec3(1._f) * reflectance90);

        auto V =
            writer.declLocale("V", camera.getMember<Vec4>("camera_position").xyz() - in_position);
        auto L          = writer.declLocale<Vec3>("L", normalize(light_direction));
        auto H          = writer.declLocale<Vec3>("H", normalize(L + V));
        auto reflection = writer.declLocale("reflection", -normalize(reflect(V, N)));
        reflection.y() *= -1.f;

        auto NdotL = writer.declLocale("NdotL", clamp(dot(N, L), 0.001_f, 1._f));
        auto NdotV = writer.declLocale("NdotV", clamp(abs(dot(N, V)), 0.001_f, 1._f));
        auto NdotH = writer.declLocale("NdotH", clamp(dot(N, H), 0._f, 1._f));
        auto LdotH = writer.declLocale("LdotH", clamp(dot(L, H), 0._f, 1._f));
        auto VdotH = writer.declLocale("VdotH", clamp(dot(V, H), 0._f, 1._f));

        auto alpha_roughness_pow_2 = pow(alpha_roughness, 2._f);
        auto F                     = writer.declLocale("F",
                                   vec3(specular_color.rgb() +
                                        (reflectance90 - reflectance) *
                                            pow(clamp(1.f - VdotH, 0._f, 1._f), 5._f)));

        auto attenuation_L = writer.declLocale("attenuation_L",
                                               2.f * NdotL /
                                                   (NdotL + sqrt(alpha_roughness_pow_2 +
                                                                 (1.f - alpha_roughness_pow_2)) *
                                                                pow(NdotL, 2._f)));
        auto attenuation_V = writer.declLocale("attenuation_V",
                                               2.f * NdotV /
                                                   (NdotV + sqrt(alpha_roughness_pow_2 +
                                                                 (1.f - alpha_roughness_pow_2)) *
                                                                pow(NdotV, 2._f)));

        auto G = writer.declLocale("G", attenuation_L * attenuation_V);

        auto f = (NdotH * alpha_roughness_pow_2 - NdotH) * NdotH + 1.f;
        auto D = writer.declLocale("D", alpha_roughness_pow_2 / (3.141592653589_f * f * f));

        auto diffuse_contribution =
            writer.declLocale("diffuse_contribution",
                              (1.f - F) * (diffuse / vec3(3.141592653589_f)));
        auto specular_contribution =
            writer.declLocale("specular_contribution", F * G * D / (4.f * NdotL * NdotV));

        frag_color = vec4(NdotL * light_color * (diffuse_contribution + specular_contribution),
                          base_color.a());

        frag_color += vec4((vec3(1._f) * diffuse) + (vec3(1._f) * specular_color), 1.f);

        if (mesh_material.hasAmbiantOcclusionMap()) {
            auto ao =
                writer.declLocale<Float>("ao", texture(ambiant_occlusion_sampler, in_texcoord).r());
            auto strength = material.getMember<Float>("material_ambiant_occlusion_factor");
            frag_color    = vec4(mix(frag_color.rgb(),
                                  frag_color.rgb() * ao,
                                  vec3(strength, strength, strength)),
                              frag_color.a());
        }

        frag_color += emissive_color;

        // frag_color = vec4(F, 1.f);
    });

    auto spirv = spirv::serialiseSpirv(writer.getShader());

    shader_pool.create(std::string { name },
                       spirv,
                       render::ShaderStage::Fragment,
                       m_engine->device());
}
