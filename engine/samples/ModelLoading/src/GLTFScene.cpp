#include "GLTFScene.hpp"

/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Surface.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/mesh/StaticMesh.hpp>

#include <storm/engine/mesh/Model.hpp>

#include <storm/engine/scene/FPSCamera.hpp>

#include <storm/engine/framegraph/FrameGraph.hpp>
#include <storm/engine/framegraph/FramePass.hpp>
#include <storm/engine/framegraph/FramePassBuilder.hpp>
#include <storm/engine/framegraph/FramePassResources.hpp>

using namespace storm;
using storm::log::operator""_module;

static constexpr auto LOG_MODULE = "ModelLoading"_module;

////////////////////////////////////////
////////////////////////////////////////
GLTFScene::GLTFScene(engine::Engine &engine,
                     const window::Window &window,
                     std::filesystem::path model_filepath)
    : engine::Scene { engine }, m_window { &window }, m_input_handler { window } {
    m_camera = std::make_unique<engine::FPSCamera>(
        *m_engine,
        m_engine->surface().extent().convertTo<core::Extentf>());
    m_camera->setPosition({ 0.f, 0.f, 1.f });
    m_camera->setRotation({ 180.f, 0.f, 0.f });

    setCamera(*m_camera);

    m_model = createModelPtr();
    m_model->load(model_filepath);

    if (!m_model->loaded()) {
        log::LogHandler::flog(LOG_MODULE, "Failed to load {}", model_filepath.string());
        std::exit(EXIT_FAILURE);
    }
    log::LogHandler::ilog(LOG_MODULE, "{} loaded", model_filepath.string());

    m_meshes = m_model->createMeshes();

    for (auto &mesh : m_meshes) {
        const auto &bounding_box = mesh.boundingBox();

        const auto max_length =
            std::max(bounding_box.extent.width,
                     std::max(bounding_box.extent.height, bounding_box.extent.depth));

        const auto scale = (1.f / max_length) * 0.5f;

        auto translation = core::Vector3f {};
        translation.x    = -(bounding_box.max.x + bounding_box.min.x) / 2.f;
        translation.y    = -(bounding_box.max.y + bounding_box.min.y) / 2.f;
        translation.z    = -(bounding_box.max.z + bounding_box.min.z) / 2.f;

        auto &transform = mesh.transform();
        transform.setScale(scale, scale, scale);
        transform.setPosition(translation);
    }

    m_engine->debugGUI().setSkipFrameCount(40);

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
        log::LogHandler::elog(LOG_MODULE, "Wireframe is not supported on this GPU");
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
    const auto samples =
        (isMSAAEnabled()) ? render::SampleCountFlag::C1_BIT : m_engine->maxSampleCount();

    setMSAASampleCount(samples);
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
void GLTFScene::update(float time) {
    const auto extent = m_engine->surface().extent().convertTo<core::Extenti>();

    if (m_rotate_mesh) {
        for (auto &mesh : m_meshes) { mesh.transform().setPitch(time * 90.f); }
    }

    if (!m_freeze_camera) {
        auto camera_inputs = engine::FPSCamera::Inputs {};

        if (m_input_handler.isKeyPressed(window::Key::Z)) camera_inputs.up = true;
        if (m_input_handler.isKeyPressed(window::Key::S)) camera_inputs.down = true;
        if (m_input_handler.isKeyPressed(window::Key::Q)) camera_inputs.left = true;
        if (m_input_handler.isKeyPressed(window::Key::D)) camera_inputs.right = true;

        const auto position = [&camera_inputs, &extent, this]() {
            auto position = m_input_handler.getMousePositionOnWindow();

            if (position->x < 0 || position->x > extent.w) {
                position->x                = extent.w / 2;
                camera_inputs.mouse_ignore = true;
            }
            if (position->y < 0 || position->y > extent.h) {
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

    struct ColorPassData {
        engine::FramePassTextureID depth;
        engine::FramePassTextureID msaa;
        engine::FramePassTextureID output;
    };

    const auto sample_count = m_engine->maxSampleCount();

    const auto depth_descriptor = engine::FrameGraphTexture::Descriptor {
        .type    = render::TextureType::T2D,
        .format  = render::PixelFormat::Depth32F_Stencil8,
        .extent  = surface.extent(),
        .samples = (isMSAAEnabled()) ? sample_count : render::SampleCountFlag::C1_BIT,
        .usage   = render::TextureUsage::Depth_Stencil_Attachment
    };
    const auto msaa_descriptor =
        engine::FrameGraphTexture::Descriptor { .type    = render::TextureType::T2D,
                                                .format  = surface.pixelFormat(),
                                                .extent  = surface.extent(),
                                                .samples = sample_count,
                                                .usage   = render::TextureUsage::Color_Attachment };

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

        for (auto &mesh : m_meshes)
            mesh.render(cmb, resources.renderPass(), std::move(bindables), mesh_state);
    };

    auto &color_pass =
        framegraph.addPass<ColorPassData>("ColorPass", std::move(setup), std::move(execute));

    color_pass.setCullImune(true);

    if (m_show_debug_gui) m_engine->doInitDebugGUIPasses(color_pass.data().output, framegraph);
}
