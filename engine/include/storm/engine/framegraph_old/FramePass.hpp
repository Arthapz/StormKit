// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/framegraph/FramePassResource.hpp>
#include <storm/engine/framegraph/Fwd.hpp>

namespace storm::engine {
    class STORM_PUBLIC FramePassBase: public core::NonCopyable {
      public:
        explicit FramePassBase(std::string name, bool secondary_command_buffer) noexcept;
        virtual ~FramePassBase() = 0;

        FramePassBase(FramePassBase &&);
        FramePassBase &operator=(FramePassBase &&);

        virtual void setup(FramePassBuilder &builder)                                         = 0;
        virtual void execute(const FramePassResources &resources, render::CommandBuffer &cmb) = 0;

        inline std::string_view name() const noexcept;

        inline FramePassTextureResourceObserverPtrConstSpan createTextures() const noexcept;
        inline FramePassTextureResourceObserverPtrConstSpan readTextures() const noexcept;
        inline FramePassTextureResourceObserverPtrConstSpan writeTextures() const noexcept;

        inline FramePassBufferResourceObserverPtrConstSpan createBuffers() const noexcept;
        inline FramePassBufferResourceObserverPtrConstSpan readBuffers() const noexcept;
        inline FramePassBufferResourceObserverPtrConstSpan writeBuffers() const noexcept;

        inline bool secondaryCommandBuffer() const noexcept;

        inline void setCullImune(bool imune) noexcept;
        inline bool isCullImune() const noexcept;

      private:
        std::string m_name;

        FramePassTextureResourceObserverPtrArray m_create_textures;
        FramePassBufferResourceObserverPtrArray m_create_buffers;

        FramePassTextureResourceObserverPtrArray m_read_textures;
        FramePassBufferResourceObserverPtrArray m_read_buffers;

        FramePassTextureResourceObserverPtrArray m_write_textures;
        FramePassBufferResourceObserverPtrArray m_write_buffers;

        bool m_secondary_command_buffer;

        bool m_cull_imune = false;

        friend class FramePassBuilder;
    };

    template<typename PassData>
    class FramePass: public FramePassBase {
      public:
        using SetupCallback   = std::function<void(FramePassBuilder &, PassData &)>;
        using ExecuteCallback = std::function<
            void(const PassData &, const FramePassResources &, render::CommandBuffer &)>;

        FramePass(std::string name,
                  SetupCallback &&setup_callback,
                  ExecuteCallback &&execute_callback,
                  bool secondary_command_buffer = false) noexcept;
        ~FramePass() override;

        FramePass(FramePass &&);
        FramePass &operator=(FramePass &&);

        void setup(FramePassBuilder &builder) override;
        void execute(const FramePassResources &resources, render::CommandBuffer &cmb) override;

        inline const PassData &data() const noexcept;

      private:
        PassData m_data;

        SetupCallback m_setup_callback;
        ExecuteCallback m_execute_callback;
    };
} // namespace storm::engine

#include "FramePass.inl"
