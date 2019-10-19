/////////// - imgui - ///////////
#include <iterator>

/////////// - imgui - ///////////
#include "imgui/imgui.hpp"
#include "imgui/imgui_impl_vulkan.hpp"
#include "imgui/imgui_widget_flamegraph.hpp"

/////////// - StormKit::window - ///////////
#include <storm/window/InputHandler.hpp>
#include <storm/window/Window.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Instance.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Queue.hpp>

#include <storm/render/pipeline/DescriptorPool.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/core/Profiler.hpp>

#include <storm/engine/core/DebugGUI.hpp>

using namespace storm;
using namespace storm::engine;

void profilerGetValue(float *start,
                      float *end,
                      ImU8 *level,
                      const char **caption,
                      const void *data,
                      int idx) {
    namespace Chrono = std::chrono;

    auto entry = reinterpret_cast<const Profiler::Entry *>(data);
    auto it    = std::cbegin(entry->scopes);
    if (it == std::cend(entry->scopes)) return;

    std::advance(it, idx);

    const auto &[name, scope] = *it;

    if (start) {
        const auto time =
            Chrono::duration<float, std::milli> { scope.start - entry->start }.count();
        *start = time;
    }

    if (end) {
        const auto time = Chrono::duration<float, std::milli> { scope.end - entry->start }.count();
        *end            = time;
    }

    if (level) *level = scope.level;

    if (caption) *caption = std::data(name);
}

/////////////////////////////////////
/////////////////////////////////////
DebugGUI::DebugGUI(Engine &engine) : m_engine { &engine } {
    const auto extent = m_engine->surface().extent().convertTo<core::Extentf>();

    ImGui::CreateContext();
    auto &io         = ImGui::GetIO();
    io.DisplaySize.x = extent.width;
    io.DisplaySize.y = extent.height;
    io.Fonts->AddFontDefault();
    io.Fonts->Build();

    ImGui::StyleColorsDark();

    std::fill(core::ranges::begin(m_frame_times), core::ranges::end(m_frame_times), 0.f);
}

/////////////////////////////////////
/////////////////////////////////////
DebugGUI::~DebugGUI() {
    ImGui_ImplVulkan_Shutdown();

    ImGui::DestroyContext();
}

/////////////////////////////////////
/////////////////////////////////////
DebugGUI::DebugGUI(DebugGUI &&) = default;

/////////////////////////////////////
/////////////////////////////////////
DebugGUI &DebugGUI::operator=(DebugGUI &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void check_vk_result(VkResult err) {
}

/////////////////////////////////////
/////////////////////////////////////
void DebugGUI::init(const render::RenderPass &render_pass) {
    const auto &instance        = m_engine->instance();
    const auto &device          = m_engine->device();
    auto &pipeline_cache        = m_engine->pipelineCache();
    const auto &descriptor_pool = m_engine->descriptorPool();
    const auto &queue           = device.graphicsQueue();
    m_buffer_count              = m_engine->surface().bufferingCount();

    auto init_info =
        ImGui_ImplVulkan_InitInfo { .Instance        = instance.vkHandle(),
                                    .PhysicalDevice  = device.physicalDevice().vkHandle(),
                                    .Device          = device.vkHandle(),
                                    .Dispatcher      = device.vkDispatcher(),
                                    .QueueFamily     = 0,
                                    .Queue           = queue.vkHandle(),
                                    .PipelineCache   = pipeline_cache.vkHandle(),
                                    .DescriptorPool  = descriptor_pool.vkHandle(),
                                    .MinImageCount   = m_buffer_count,
                                    .ImageCount      = m_buffer_count,
                                    .Allocator       = VK_NULL_HANDLE,
                                    .CheckVkResultFn = check_vk_result };

    m_sub_command_buffers =
        queue.createCommandBufferPtrs(m_buffer_count, storm::render::CommandBufferLevel::Secondary);

    ImGui_ImplVulkan_Init(&init_info, render_pass.vkHandle());

    auto fence  = device.createFence();
    auto cmb    = queue.createCommandBuffer();
    auto cmb_vk = cmb.vkHandle();

    const auto begin_info =
        vk::CommandBufferBeginInfo {}.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmb_vk.begin(begin_info, device.vkDispatcher());

    ImGui_ImplVulkan_CreateFontsTexture(cmb_vk);

    cmb_vk.end(device.vkDispatcher());

    queue.submit(storm::core::makeConstObservers(cmb), {}, {}, storm::core::makeObserver(fence));

    fence.wait();

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

/////////////////////////////////////
/////////////////////////////////////
void DebugGUI::update(const storm::window::Window &window) {
    ImGui_ImplVulkan_NewFrame();

    m_current_cpu_time = m_engine->getCPUTime();

    ImGui::NewFrame();

    auto &io     = ImGui::GetIO();
    io.DeltaTime = m_current_cpu_time;

    io.MouseDown[0] = window::InputHandler::isMouseButtonPressed(window::MouseButton::LEFT);
    io.MouseDown[1] = window::InputHandler::isMouseButtonPressed(window::MouseButton::RIGHT);
    io.MouseDown[2] = window::InputHandler::isMouseButtonPressed(window::MouseButton::MIDDLE);

    const auto position = storm::window::InputHandler::getMousePosition(window);

    io.MousePos = { static_cast<float>(position->x), static_cast<float>(position->y) };

    const auto cpu_time = (core::realIsEqual(m_current_cpu_time, 0.f)) ? 1.f : m_current_cpu_time;
    m_frame_times[m_frame_time_pointer] = 1.f / cpu_time;
}

/////////////////////////////////////
/////////////////////////////////////
void DebugGUI::render(storm::render::CommandBuffer &cmb, const render::RenderPass &render_pass) {
    namespace Chrono = std::chrono;

    const auto &profiler      = m_engine->profiler();
    const auto &current_entry = profiler.entries()[profiler.currentEntryIndex()];

    const auto &device = m_engine->device();
    m_current_buffer++;
    if (m_current_buffer >= m_buffer_count) m_current_buffer = 0;

    bool foo = true;
    ImGui::Begin("Debug window",
                 &foo,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::BeginGroup();

    for (auto i : m_frame_times) m_max_fps = std::max(m_max_fps, static_cast<core::UInt32>(i));

    const auto title = fmt::format("{}\n\n\n0", m_max_fps);
    ImGui::PlotHistogram(title.c_str(),
                         std::data(m_frame_times),
                         std::size(m_frame_times),
                         0,
                         "Frame times",
                         0,
                         m_max_fps,
                         { 400, 60 });
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);

    static constexpr auto max = std::numeric_limits<float>::max();
    ImGuiWidgetFlameGraph::PlotFlame("CPU",
                                     &profilerGetValue,
                                     &current_entry,
                                     std::size(current_entry.scopes),
                                     0,
                                     "Main Thread",
                                     max,
                                     max,
                                     { 400, 60 });
    ImGui::EndGroup();

    ImGui::BeginGroup();
    if (ImGui::CollapsingHeader("Detail CPU")) {
        const auto full_frame =
            Chrono::duration<float, std::milli> { current_entry.end - current_entry.start }.count();
        ImGui::Text("Full Frame: %.3f ms", full_frame);

        for (const auto &[name, scope] : current_entry.scopes) {
            const auto time =
                Chrono::duration<float, std::milli> { scope.end - scope.start }.count();
            auto str = std::string { "%s: %.3f ms" };
            for (auto i = 0u; i < scope.level; ++i) str.insert(0, "   ");

            ImGui::Text(std::data(str), std::data(name), time);
        }
    }
    ImGui::EndGroup();

    ImGui::End();
    ImGui::Render();

    auto &sub_cmb = *m_sub_command_buffers[m_current_buffer];
    sub_cmb.reset();

    auto sub_cmb_vk = sub_cmb.vkHandle();

    const auto inheritance_info =
        vk::CommandBufferInheritanceInfo {}.setRenderPass(render_pass.vkHandle()).setSubpass(0);

    const auto begin_info = vk::CommandBufferBeginInfo {}
                                .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit |
                                          vk::CommandBufferUsageFlagBits::eRenderPassContinue)
                                .setPInheritanceInfo(&inheritance_info);
    sub_cmb_vk.begin(begin_info, device.vkDispatcher());

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), sub_cmb_vk);

    sub_cmb_vk.end(device.vkDispatcher());

    cmb.executeSubCommandBuffers({ sub_cmb });

    m_frame_time_pointer++;
    if (m_frame_time_pointer >= std::size(m_frame_times)) m_frame_time_pointer = 0u;
}
