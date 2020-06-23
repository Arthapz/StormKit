// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <filesystem>
#include <unordered_map>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/pipeline/GraphicsPipelineState.hpp>

namespace storm::render {
    class STORM_PUBLIC PipelineCache: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Pipeline_Cache;

        explicit PipelineCache(const Device &devicen,
                               std::filesystem::path cache_path = "./pipeline_cache.bin");
        ~PipelineCache();

        PipelineCache(PipelineCache &&);
        PipelineCache &operator=(PipelineCache &&);

        render::GraphicsPipeline &getPipeline(const GraphicsPipelineState &state,
                                              const RenderPass &render_pass);

        inline vk::PipelineCache vkPipelineCache() const noexcept;
        inline operator vk::PipelineCache() const noexcept;
        inline vk::PipelineCache vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

      private:
        bool has(const GraphicsPipelineState &state, const RenderPassDescription &description) const noexcept;

        void createNewPipelineCache();
        void readPipelineCache();
        void saveCache();

        static constexpr auto MAGIC   = core::UInt32 { 0xDEADBEEF };
        static constexpr auto VERSION = core::UInt32 { 1u };
        struct SerializedCache {
            struct {
                core::UInt32 magic;
                core::UInt32 data_size;
                core::UInt64 data_hash;
            } guard;

            struct {
                core::UInt32 version;
                core::UInt32 vendor_id;
                core::UInt32 device_id;
            } infos;

            struct {
                std::array<core::UInt8, VK_UUID_SIZE> value;
            } uuid;
        } m_serialized;


        DeviceConstObserverPtr m_device;

        vk::UniquePipelineCache m_vk_pipeline_cache;

        std::filesystem::path m_path;

        core::HashMap<GraphicsPipelineState, core::HashMap<RenderPassDescription, GraphicsPipelineOwnedPtr>> m_pipelines;
    };
} // namespace storm::render

#include "PipelineCache.inl"
