// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline std::string_view FramePassBase::name() const noexcept { return m_name; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline bool FramePassBase::secondaryCommandBuffer() const noexcept {
        return m_secondary_command_buffer;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline FramePassTextureResourceObserverPtrConstSpan FramePassBase::createTextures() const
        noexcept {
        return m_create_textures;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline FramePassTextureResourceObserverPtrConstSpan FramePassBase::readTextures() const
        noexcept {
        return m_read_textures;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline FramePassTextureResourceObserverPtrConstSpan FramePassBase::writeTextures() const
        noexcept {
        return m_write_textures;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline FramePassBufferResourceObserverPtrConstSpan FramePassBase::createBuffers() const
        noexcept {
        return m_create_buffers;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline FramePassBufferResourceObserverPtrConstSpan FramePassBase::readBuffers() const noexcept {
        return m_read_buffers;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline FramePassBufferResourceObserverPtrConstSpan FramePassBase::writeBuffers() const
        noexcept {
        return m_write_buffers;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void FramePassBase::setCullImune(bool imune) noexcept { m_cull_imune = imune; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline bool FramePassBase::isCullImune() const noexcept { return m_cull_imune; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename PassData>
    FramePass<PassData>::FramePass(std::string name,
                                   SetupCallback &&setup_callback,
                                   ExecuteCallback &&execute_callback,
                                   bool secondary_command_buffer) noexcept
        : FramePassBase { std::move(name), secondary_command_buffer },
          m_setup_callback { std::forward<SetupCallback>(setup_callback) }, m_execute_callback {
              std::forward<ExecuteCallback>(execute_callback)
          } {}

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename PassData>
    FramePass<PassData>::~FramePass() = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename PassData>
    FramePass<PassData>::FramePass(FramePass &&) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename PassData>
    FramePass<PassData> &FramePass<PassData>::operator=(FramePass &&) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename PassData>
    void FramePass<PassData>::setup(FramePassBuilder &builder) {
        m_setup_callback(builder, m_data);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename PassData>
    void FramePass<PassData>::execute(const FramePassResources &resources,
                                      render::CommandBuffer &cmb) {
        m_execute_callback(m_data, resources, cmb);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename PassData>
    inline const PassData &FramePass<PassData>::data() const noexcept {
        return m_data;
    }
} // namespace storm::engine
