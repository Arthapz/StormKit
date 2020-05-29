#include "GLTFScene.hpp"

/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Surface.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/scene/FPSCamera.hpp>

#include <storm/engine/framegraph/FrameGraph.hpp>
#include <storm/engine/framegraph/FramePass.hpp>
#include <storm/engine/framegraph/FramePassBuilder.hpp>
#include <storm/engine/framegraph/FramePassResources.hpp>

using namespace storm;
using storm::log::operator""_module;

static constexpr auto GLTF_LOG_MODULE = "ModelLoading"_module;

////////////////////////////////////////
////////////////////////////////////////
GLTFScene::GLTFScene(engine::Engine &engine,
                     const window::Window &window,
                     std::filesystem::path model_filepath)
    : engine::PBRScene { engine }, m_window { &window }, m_input_handler { window } {
    m_camera = std::make_unique<engine::FPSCamera>(
        *m_engine,
        m_engine->surface().extent().convertTo<core::Extentf>());
    m_camera->setPosition({ 0.f, 0.f, -0.5f });
    m_camera->setRotation({ 0.f, 0.f, 0.f });

    setCamera(*m_camera);

    m_model = createModelPtr();
    m_model->load(model_filepath);

    if (!m_model->loaded()) {
        log::LogHandler::flog(GLTF_LOG_MODULE, "Failed to load {}", model_filepath.string());
        std::exit(EXIT_FAILURE);
    }
    log::LogHandler::ilog(GLTF_LOG_MODULE, "{} loaded", model_filepath.string());

    m_meshes = m_model->createMeshes();

    auto bounding_box = engine::BoundingBox {};
    bounding_box.min  = { 0.f, 0.f, 0.f };
    bounding_box.max  = { 0.f, 0.f, 0.f };

    for (const auto &mesh : m_meshes) {
        const auto &_bounding_box = mesh.boundingBox();

        bounding_box.min = core::min(bounding_box.min, _bounding_box.min);
        bounding_box.max = core::max(bounding_box.max, _bounding_box.max);
    }

    const auto extent = bounding_box.max - bounding_box.min;

    bounding_box.extent.width  = extent.x;
    bounding_box.extent.height = extent.y;
    bounding_box.extent.depth  = extent.z;

    const auto max_length =
        std::max(bounding_box.extent.width,
                 std::max(bounding_box.extent.height, bounding_box.extent.depth));

    const auto scale = (1.f / max_length) * 0.5f;

    auto translation = -bounding_box.min;
    translation +=
        -0.5f *
        core::Vector3f { bounding_box.extent.w, bounding_box.extent.h, bounding_box.extent.d };

    for (auto &mesh : m_meshes) {
        auto &transform = mesh.transform();
        transform.setScale(scale, scale, scale);
        transform.setPosition(translation);
    }

    m_engine->debugGUI().setSkipFrameCount(40);

    m_cube_map = createCubeMapPtr();

    auto &cube_map_texture = texturePool().create("CubeMap",
                                                  m_engine->device(),
                                                  render::TextureType::T2D,
                                                  render::TextureCreateFlag::Cube_Compatible);
    cube_map_texture.loadFromKTX(EXAMPLES_DATA_DIR "textures/cubemap.ktx");

    m_cube_map->setTexture(cube_map_texture,
                           { .level_count = cube_map_texture.mipLevels(),
                             .layer_count = cube_map_texture.layers() });

    enableDepthTest(true);
    toggleMSAA();
}

////////////////////////////////////////
////////////////////////////////////////
GLTFScene::~GLTFScene() = default;

////////////////////////////////////////
////////////////////////////////////////
GLTFScene::GLTFScene(GLTFScene &&) = default;

////////////////////////////////////////
////////////////////////////////////////
GLTFScene &GLTFScene::operator=(GLTFScene &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void GLTFScene::toggleWireframe() {
    const auto &capabilities = m_engine->device().physicalDevice().capabilities();
    if (!capabilities.features.fill_Mode_non_solid) {
        log::LogHandler::elog(GLTF_LOG_MODULE, "Wireframe is not supported on this GPU");
        return;
    }

    m_wireframe = !m_wireframe;

    for (auto &mesh : m_meshes)
        for (auto &submesh : mesh.subMeshes())
            submesh.materialInstance().setWireFrameEnabled(m_wireframe);
}

////////////////////////////////////////
////////////////////////////////////////
void GLTFScene::toggleMSAA() noexcept {
    if (m_engine->maxSampleCount() != render::SampleCountFlag::C1_BIT) {
        if (m_sample_count == m_engine->maxSampleCount())
            m_sample_count = render::SampleCountFlag::C1_BIT;

        m_sample_count = core::nextValue(m_sample_count);
    }

    setMSAASampleCount(m_sample_count);
}

////////////////////////////////////////
////////////////////////////////////////
void GLTFScene::setDebugView(engine::PBRMaterialInstance::DebugView debug_index) {
    for (auto &mesh : m_meshes) {
        for (auto &submesh : mesh.subMeshes()) {
            auto &pbr_material =
                static_cast<engine::PBRMaterialInstance &>(submesh.materialInstance());
            pbr_material.setDebugView(debug_index);
        }
    }
}

////////////////////////////////////////
////////////////////////////////////////
void GLTFScene::update(float delta) {
    static constexpr auto degree_per_sec = 7.f;

    const auto extent = m_engine->surface().extent().convertTo<core::Extenti>();

    if (m_rotate_mesh) {
        const auto degrees = degree_per_sec * delta;
        for (auto &mesh : m_meshes) { mesh.transform().rotateYaw(degrees); }
    }

    if (!m_freeze_camera) {
        auto camera_inputs = engine::FPSCamera::Inputs {};

        if (m_input_handler.isKeyPressed(window::Key::Z)) camera_inputs.up = true;
        if (m_input_handler.isKeyPressed(window::Key::S)) camera_inputs.down = true;
        if (m_input_handler.isKeyPressed(window::Key::Q)) camera_inputs.left = true;
        if (m_input_handler.isKeyPressed(window::Key::D)) camera_inputs.right = true;

        const auto position = [&camera_inputs, &extent, this]() {
            auto position = m_input_handler.getMousePositionOnWindow();

            if (position->x <= 5 || position->x > (extent.w - 5)) {
                position->x                = extent.w / 2;
                camera_inputs.mouse_ignore = true;
            }
            if (position->y <= 5 || position->y > (extent.h - 5)) {
                position->y                = extent.h / 2;
                camera_inputs.mouse_ignore = true;
            }

            m_input_handler.setMousePositionOnWindow(position);

            return position;
        }();

        camera_inputs.mouse_updated = true;
        camera_inputs.x_mouse       = static_cast<float>(position->x);
        camera_inputs.y_mouse       = static_cast<float>(position->y);

        m_camera->feedInputs(camera_inputs);
    }

    if (m_show_debug_gui) {
        auto &debug_gui = m_engine->debugGUI();
        debug_gui.update(*m_window);
    }
}

////////////////////////////////////////
////////////////////////////////////////
void GLTFScene::doRenderScene(storm::engine::FrameGraph &framegraph,
                              storm::engine::FramePassTextureID backbuffer,
                              std::vector<storm::engine::BindableBaseConstObserverPtr> bindables,
                              storm::render::GraphicsPipelineState &state) {
    const auto &surface = m_engine->surface();

    if (m_cube_map_dirty) {
        insertGenerateCubeMapPass(framegraph, *m_cube_map);
        m_cube_map_dirty = false;
    }

    struct ColorPassData {
        engine::FramePassTextureID depth;
        engine::FramePassTextureID msaa;
        engine::FramePassTextureID output;
    };

    const auto depth_descriptor = engine::FrameGraphTexture::Descriptor {
        .type    = render::TextureType::T2D,
        .format  = render::PixelFormat::Depth32F_Stencil8,
        .extent  = surface.extent(),
        .samples = m_sample_count,
        .usage   = render::TextureUsage::Depth_Stencil_Attachment
    };
    const auto msaa_descriptor =
        engine::FrameGraphTexture::Descriptor { .type    = render::TextureType::T2D,
                                                .format  = surface.pixelFormat(),
                                                .extent  = surface.extent(),
                                                .samples = m_sample_count,
                                                .usage   = render::TextureUsage::Color_Attachment };

    bindables.emplace_back(core::makeConstObserver(m_data));

    const auto setup =
        [&backbuffer, &depth_descriptor, &msaa_descriptor, this](engine::FramePassBuilder &builder,
                                                                 ColorPassData &pass_data) {
            pass_data.depth =
                builder.create<engine::FrameGraphTexture>("Depth", std::move(depth_descriptor));

            if (isMSAAEnabled()) {
                pass_data.msaa =
                    builder.create<engine::FrameGraphTexture>("MSAA", std::move(msaa_descriptor));
                pass_data.msaa  = builder.write(pass_data.msaa);
                pass_data.depth = builder.write(pass_data.depth);

                pass_data.output = builder.resolve(backbuffer);
            } else {
                pass_data.output = builder.write(backbuffer);
                pass_data.depth  = builder.write(pass_data.depth);
            }
        };

    const auto execute = [this, bindables, state]([[maybe_unused]] const ColorPassData &pass_data,
                                                  const engine::FramePassResources &resources,
                                                  render::CommandBuffer &cmb) {
        auto mesh_state = state;

        mesh_state.color_blend_state.attachments = {
            render::GraphicsPipelineColorBlendAttachmentState {}
        };

        m_cube_map->render(cmb, resources.renderPass(), bindables, mesh_state);
        for (auto &mesh : m_meshes) mesh.render(cmb, resources.renderPass(), bindables, mesh_state);
    };

    auto &color_pass =
        framegraph.addPass<ColorPassData>("ColorPass", std::move(setup), std::move(execute));

    color_pass.setCullImune(true);

    if (m_show_debug_gui) m_engine->doInitDebugGUIPasses(color_pass.data().output, framegraph);
}
