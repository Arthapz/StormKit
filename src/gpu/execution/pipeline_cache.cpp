// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/try_expected.hpp>

#include <stormkit/log/log_macro.hpp>

#include <stormkit/gpu/vulkan.hpp>

module stormkit.gpu.execution;

import std;

import stormkit.core;
import stormkit.log;

import stormkit.gpu.core;

namespace stdr = std::ranges;

LOGGER("stormkit.gpu")

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    inline auto sys_to_load_error(SystemError error) noexcept -> PipelineCache::LoadSaveError {
        return PipelineCache::LoadSaveError { { error } };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    inline auto result_to_load_error(Result error) noexcept -> PipelineCache::LoadSaveError {
        return PipelineCache::LoadSaveError { { error } };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    PipelineCache::~PipelineCache() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::create_new_pipeline_cache(const Device& device) noexcept -> LoadSaveExpected<void> {
        const auto physical_device_infos = device.physical_device().info();

        m_serialized.guard.magic     = MAGIC;
        m_serialized.guard.data_size = 0u;
        m_serialized.guard.data_hash = 0u;

        m_serialized.infos.version   = VERSION;
        m_serialized.infos.vendor_id = physical_device_infos.vendor_id;
        m_serialized.infos.device_id = physical_device_infos.device_id;

        stdr::copy(physical_device_infos.pipeline_cache_uuid, stdr::begin(m_serialized.uuid.value));

        const auto create_info = VkPipelineCacheCreateInfo {
            .sType           = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            .pNext           = nullptr,
            .flags           = 0,
            .initialDataSize = 0,
            .pInitialData    = nullptr,
        };

        m_vk_handle = Try(vk_call<VkPipelineCache>(m_vk_device_table->vkCreatePipelineCache, m_vk_device, &create_info, nullptr)
                            .transform_error(monadic::from_vk<Result>())
                            .transform_error(result_to_load_error));

        Ret({});
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::read_pipeline_cache(const Device& device) noexcept -> LoadSaveExpected<void> {
        if (not std::filesystem::exists(m_path)) Ret(create_new_pipeline_cache(device));

        const auto physical_device_infos = device.physical_device().info();

        auto file = Try(io::File::open(m_path, io::Access::READ).transform_error(sys_to_load_error));
        Try(file.read_to(as_bytes_mut(m_serialized.guard)).transform_error(sys_to_load_error));
        Try(file.read_to(as_bytes_mut(m_serialized.infos)).transform_error(sys_to_load_error));
        Try(file.read_to(as_bytes_mut(m_serialized.uuid.value)).transform_error(sys_to_load_error));

        if (m_serialized.guard.magic != MAGIC) {
            elog("Invalid pipeline cache magic number, have {}, expected: {}", m_serialized.guard.magic, MAGIC);

            Ret(create_new_pipeline_cache(device));
        }

        if (m_serialized.infos.version != VERSION) {
            elog("Mismatch pipeline cache version, have {}, expected: {}", m_serialized.infos.version, VERSION);

            Ret(create_new_pipeline_cache(device));
        }

        if (m_serialized.infos.vendor_id != physical_device_infos.vendor_id) {
            elog("Mismatch pipeline cache vendor id, have {:#06x}, expected: {:#06x}",
                 m_serialized.infos.vendor_id,
                 physical_device_infos.vendor_id);

            Ret(create_new_pipeline_cache(device));
        }

        if (m_serialized.infos.device_id != physical_device_infos.device_id) {
            elog("Mismatch pipeline cache device id, have {:#06x}, expected: {:#06x}",
                 m_serialized.infos.device_id,
                 physical_device_infos.device_id);

            Ret(create_new_pipeline_cache(device));
        }

        if (not stdr::equal(m_serialized.uuid.value, physical_device_infos.pipeline_cache_uuid)) {
            Ret(create_new_pipeline_cache(device));
        }

        auto data = std::vector<Byte> {};
        data.resize(m_serialized.guard.data_size);

        Try(io::read_to(m_path, data).transform_error(sys_to_load_error));

        const auto create_info = VkPipelineCacheCreateInfo {
            .sType           = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            .pNext           = nullptr,
            .flags           = 0,
            .initialDataSize = as<u32>(stdr::size(data)),
            .pInitialData    = stdr::data(data),
        };

        m_vk_handle = Try(vk_call<VkPipelineCache>(m_vk_device_table->vkCreatePipelineCache, m_vk_device, &create_info, nullptr)
                            .transform_error(monadic::from_vk<Result>())
                            .transform_error(result_to_load_error));

        Ret({});
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::save_cache() noexcept -> LoadSaveExpected<void> {
        auto data = Try((vk_enumerate<Byte, usize>(m_vk_device_table->vkGetPipelineCacheData, m_vk_device, m_vk_handle)
                           .transform_error(monadic::from_vk<Result>())
                           .transform_error(result_to_load_error)));
        m_serialized.guard.data_size = stdr::size(data);
        m_serialized.guard.data_hash = 0u;

        hash_combine(m_serialized.guard.data_hash, data);

        auto file = Try(io::File::open(m_path, io::Access::WRITE).transform_error(sys_to_load_error));

        Try(file.write(as_bytes(m_serialized.infos)).transform_error(sys_to_load_error));
        Try(file.write(as_bytes(m_serialized.uuid.value)).transform_error(sys_to_load_error));
        Try(file.write(as_bytes(data)).transform_error(sys_to_load_error));

        Ret({});
    }
} // namespace stormkit::gpu
