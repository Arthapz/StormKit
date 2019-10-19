// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/log/LogHandler.hpp>

#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>

#include <storm/render/pipeline/GraphicsPipeline.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>

#include <fstream>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
PipelineCache::PipelineCache(const Device &device, std::filesystem::path path)
    : m_device { &device }, m_path { std::move(path) } {
    if (std::filesystem::exists(m_path)) readPipelineCache();
    else
        createNewPipelineCache();
}

/////////////////////////////////////
/////////////////////////////////////
PipelineCache::~PipelineCache() {
    saveCache();
};

/////////////////////////////////////
/////////////////////////////////////
PipelineCache::PipelineCache(PipelineCache &&) = default;

/////////////////////////////////////
/////////////////////////////////////
PipelineCache &PipelineCache::operator=(PipelineCache &&) = default;

GraphicsPipeline &PipelineCache::getPipeline(const GraphicsPipelineState &state,
                                             const RenderPass &pass) {
    if (!has(state)) {
        m_pipelines.emplace(state,
                            m_device->createGraphicsPipelinePtr(core::makeConstObserver(this)));
        auto &pipeline = m_pipelines[state];

        pipeline->setState(state);
        pipeline->setRenderPass(pass);
        pipeline->build();
    }

    return *m_pipelines[state];
}

bool PipelineCache::has(const GraphicsPipelineState &state) const noexcept {
    auto it = m_pipelines.find(state);

    return it != std::cend(m_pipelines);
}

void PipelineCache::createNewPipelineCache() {
    using log::operator"" _module;

    const auto module = "Vulkan"_module;

    log::LogHandler::ilog(module, "Creating new pipeline cache at {}", m_path.string());

    const auto physical_device_infos = m_device->physicalDevice().info();

    m_serialized.guard.magic     = MAGIC;
    m_serialized.guard.data_size = 0u;
    m_serialized.guard.data_hash = 0u;

    m_serialized.infos.version   = VERSION;
    m_serialized.infos.vendor_id = physical_device_infos.vendor_id;
    m_serialized.infos.device_id = physical_device_infos.device_id;

    std::copy(core::ranges::begin(physical_device_infos.pipeline_cache_uuid),
              core::ranges::end(physical_device_infos.pipeline_cache_uuid),
              core::ranges::begin(m_serialized.uuid.value));

    const auto create_info = vk::PipelineCacheCreateInfo {};
    m_vk_pipeline_cache    = m_device->createVkPipelineCache(std::move(create_info));
}

void PipelineCache::readPipelineCache() {
    using log::operator"" _module;

    const auto module = "Vulkan"_module;

    const auto physical_device_infos = m_device->physicalDevice().info();

    storm::log::LogHandler::ilog(module, "Loading pipeline cache {}", m_path.string());

    auto stream = std::ifstream { m_path.string(), std::ios::binary };

    stream.read(reinterpret_cast<char *>(&m_serialized.guard), sizeof(m_serialized.guard));
    stream.read(reinterpret_cast<char *>(&m_serialized.infos), sizeof(m_serialized.infos));
    stream.read(reinterpret_cast<char *>(std::data(m_serialized.uuid.value)),
                std::size(m_serialized.uuid.value));

    if (m_serialized.guard.magic != MAGIC) {
        storm::log::LogHandler::elog(module,
                                     "Invalid pipeline cache magic number, have {}, expected: {}",
                                     m_serialized.guard.magic,
                                     MAGIC);

        createNewPipelineCache();
        return;
    }

    if (m_serialized.infos.version != VERSION) {
        storm::log::LogHandler::elog(module,
                                     "Mismatch pipeline cache version, have {}, expected: {}",
                                     m_serialized.infos.version,
                                     VERSION);

        createNewPipelineCache();
        return;
    }

    if (m_serialized.infos.vendor_id != physical_device_infos.vendor_id) {
        storm::log::LogHandler::elog(
            module,
            "Mismatch pipeline cache vendor id, have {:#06x}, expected: {:#06x}",
            m_serialized.infos.vendor_id,
            physical_device_infos.vendor_id);

        createNewPipelineCache();
        return;
    }

    if (m_serialized.infos.device_id != physical_device_infos.device_id) {
        storm::log::LogHandler::elog(
            module,
            "Mismatch pipeline cache device id, have {:#06x}, expected: {:#06x}",
            m_serialized.infos.device_id,
            physical_device_infos.device_id);

        createNewPipelineCache();
        return;
    }

    if (!std::equal(core::ranges::begin(m_serialized.uuid.value),
                    core::ranges::end(m_serialized.uuid.value),
                    core::ranges::begin(physical_device_infos.pipeline_cache_uuid))) {
        storm::log::LogHandler::elog(module, "Mismatch pipeline cache device UUID");

        createNewPipelineCache();
        return;
    }

    auto data = std::vector<std::byte> {};
    data.resize(m_serialized.guard.data_size);

    stream.read(reinterpret_cast<char *>(std::data(data)), std::size(data));

    const auto create_info = vk::PipelineCacheCreateInfo {}
                                 .setInitialDataSize(std::size(data))
                                 .setPInitialData(std::data(data));

    m_vk_pipeline_cache = m_device->createVkPipelineCache(std::move(create_info));
}

void PipelineCache::saveCache() {
    using log::operator"" _module;

    const auto module = "Vulkan"_module;

    CHECK_VK_ERROR_VALUE(m_device->vkDevice().getPipelineCacheData(*m_vk_pipeline_cache,
                                                                   m_device->vkDispatcher()),
                         data);

    log::LogHandler::ilog(module, "Saving pipeline cache at {}", m_path.string());

    m_serialized.guard.data_size = std::size(data);
    m_serialized.guard.data_hash = 0u;

    for (auto v : data) core::hash_combine(m_serialized.guard.data_hash, v);

    auto stream = std::ofstream { m_path.string(), std::ios::binary | std::ios::trunc };
    stream.write(reinterpret_cast<char *>(&m_serialized.guard), sizeof(m_serialized.guard));
    stream.write(reinterpret_cast<char *>(&m_serialized.infos), sizeof(m_serialized.infos));
    stream.write(reinterpret_cast<char *>(std::data(m_serialized.uuid.value)),
                 std::size(m_serialized.uuid.value));
    stream.write(reinterpret_cast<char *>(std::data(data)), std::size(data));
}