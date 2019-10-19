// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/framegraph/FramePassResource.hpp>
#include <storm/engine/framegraph/FramePassResourceHandle.hpp>

namespace storm::engine {
    class STORM_PUBLIC FramePassBase: public core::NonCopyable {
      public:
        enum class PassType { Graphics, Compute };
        explicit FramePassBase(std::string name, core::UInt32 id, PassType type) noexcept;
        virtual ~FramePassBase() = 0;

        FramePassBase(FramePassBase &&);
        FramePassBase &operator=(FramePassBase &&);

        virtual void setup(FramePassBuilder &builder)                                         = 0;
        virtual void execute(const FramePassResources &resources, render::CommandBuffer &cmb) = 0;

        inline std::string_view name() const noexcept;
        inline core::UInt32 id() const noexcept;

        inline PassType type() const noexcept;

        inline FramePassResourceHandleConstSpan creates() const noexcept;
        inline FramePassResourceHandleConstSpan reads() const noexcept;
        inline FramePassResourceHandleConstSpan writes() const noexcept;
        inline FramePassResourceHandleConstSpan samples() const noexcept;
        inline FramePassResourceHandleConstSpan resolves() const noexcept;

        inline void setCullImune(bool imune) noexcept;
        inline bool isCullImune() const noexcept;

        inline void useSubCommandBuffer(bool use) noexcept;
        inline bool isUsingSubCommandBuffer() const noexcept;

      private:
        std::string m_name;
        core::UInt32 m_id;
        PassType m_type;

        FramePassResourceHandleArray m_creates;
        FramePassResourceHandleArray m_reads;
        FramePassResourceHandleArray m_writes;
        FramePassResourceHandleArray m_samples;
        FramePassResourceHandleArray m_resolves;

        core::UInt32 m_ref_count = 0u;

        bool m_cull_imune                    = false;
        bool m_has_side_effect               = false;
        bool m_use_secondary_command_buffers = false;

        friend class FramePassBuilder;
        friend class FrameGraph;
    };

    template<typename PassData>
    class FramePass: public FramePassBase {
      public:
        using SetupCallback   = std::function<void(FramePassBuilder &, PassData &)>;
        using ExecuteCallback = std::function<
            void(const PassData &, const FramePassResources &, render::CommandBuffer &)>;

        FramePass(std::string name,
                  core::UInt32 id,
                  SetupCallback setup_callback,
                  ExecuteCallback execute_callback,
                  PassType type = PassType::Graphics) noexcept;
        ~FramePass() override;

        FramePass(FramePass &&);
        FramePass &operator=(FramePass &&);

        void setup(FramePassBuilder &builder) override;
        void execute(const FramePassResources &resources, render::CommandBuffer &cmb) override;

        inline PassData &data() noexcept;
        inline const PassData &data() const noexcept;

      private:
        PassData m_data;

        SetupCallback m_setup_callback;
        ExecuteCallback m_execute_callback;
    };
} // namespace storm::engine

#include "FramePass.inl"
