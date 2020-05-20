// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
     std::string_view FramePassBase::name() const noexcept { return m_name; }

    ////////////////////////////////////////
    ////////////////////////////////////////
     core::UInt32 FramePassBase::id() const noexcept { return m_id; }

    ////////////////////////////////////////
    ////////////////////////////////////////
     FramePassBase::PassType FramePassBase::type() const noexcept { return m_type; }

    ////////////////////////////////////////
    ////////////////////////////////////////
     FramePassResourceHandleConstSpan FramePassBase::creates() const noexcept {
        return m_creates;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
     FramePassResourceHandleConstSpan FramePassBase::reads() const noexcept {
        return m_reads;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
     FramePassResourceHandleConstSpan FramePassBase::writes() const noexcept {
        return m_writes;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
     FramePassResourceHandleConstSpan FramePassBase::samples() const noexcept {
        return m_samples;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
     FramePassResourceHandleConstSpan FramePassBase::resolves() const noexcept {
        return m_resolves;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
     void FramePassBase::setCullImune(bool imune) noexcept { m_cull_imune = imune; }

    ////////////////////////////////////////
    ////////////////////////////////////////
     bool FramePassBase::isCullImune() const noexcept { return m_cull_imune; }

    ////////////////////////////////////////
    ////////////////////////////////////////
     void FramePassBase::useSubCommandBuffer(bool use) noexcept {
        m_use_secondary_command_buffers = use;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
     bool FramePassBase::isUsingSubCommandBuffer() const noexcept {
        return m_use_secondary_command_buffers;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename PassData>
    FramePass<PassData>::FramePass(std::string name,
                                   core::UInt32 id,
                                   SetupCallback setup_callback,
                                   ExecuteCallback execute_callback,
                                   PassType type) noexcept
        : FramePassBase { std::move(name), id, type },
          m_setup_callback { std::move(setup_callback) }, m_execute_callback { std::move(
                                                              execute_callback) } {}

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
     PassData &FramePass<PassData>::data() noexcept {
        return m_data;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename PassData>
     const PassData &FramePass<PassData>::data() const noexcept {
        return m_data;
    }
} // namespace storm::engine
