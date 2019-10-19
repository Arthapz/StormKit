#pragma once

namespace storm::graphics {
    template <typename Data>
    FramePass<Data> &FrameGraphBuilder::addPass(std::string name,
                          typename FramePass<Data>::BuildCallback build_callback,
                          typename FramePass<Data>::ExecuteCallback execute_callback) {
        const auto it = std::find_if(std::cbegin(m_passes), std::cend(m_passes), [&name](const auto &pass) { return pass->name() == name;});

        STORM_EXPECTS(it == std::cend(m_passes));

        auto &pass = m_passes.emplace_back(
                                 std::make_unique<FramePass<Data>>(
                                 std::move(name),
                                 std::move(build_callback),
                                 std::move(execute_callback)));

        return static_cast<FramePass<Data>&>(*pass);
    }

    inline FrameGraphTextureResource &FrameGraphBuilder::setRetainedTexture(std::string name, FrameGraphTextureDesc desc, render::Texture &texture) {
        return m_resources.setRetainedTexture(std::move(name), std::move(desc), texture);
    }

    inline void FrameGraphBuilder::setRetainedBuffer(std::string name, FrameGraphBufferDesc desc, render::HardwareBuffer &buffer) {
        m_resources.setRetainedBuffer(std::move(name), std::move(desc), buffer);
    }
}
