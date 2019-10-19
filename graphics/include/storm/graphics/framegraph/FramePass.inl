#pragma once

namespace storm::graphics {
    template <typename Data>
    FramePass<Data>::FramePass(std::string name, typename FramePass<Data>::BuildCallback build_callback,
                                typename FramePass<Data>::ExecuteCallback execute_callback)
        : FramePassBase{std::move(name)},
          m_build_callback(std::move(build_callback)),
          m_execute_callback(std::move(execute_callback)) {}

    template <typename Data>
    FramePass<Data>::~FramePass() = default;

    template <typename Data>
    FramePass<Data>::FramePass(FramePass<Data> &&) = default;

    template <typename Data>
    FramePass<Data> &FramePass<Data>::operator=(FramePass<Data> &&) = default;

    template <typename Data>
    void FramePass<Data>::build(FramePassBuilder &builder) {
        m_build_callback(builder, m_data);
    }

    template <typename Data>
    void FramePass<Data>::execute(render::RenderPass &render_pass, render::Framebuffer &framebuffer, render::CommandBuffer &cmb) const {
        m_execute_callback(m_data, render_pass, framebuffer, cmb);
    }
}
