// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename PassData, typename SetupCallback, typename ExecuteCallback>
    FramePass<PassData> &FrameGraph::addPass(std::string name,
                                             SetupCallback &&setup_callback,
                                             ExecuteCallback &&execute_callback,
                                             FramePassBase::PassType type) {
        const auto id = m_pass_id++;

        auto &frame_pass = *m_frame_passes.emplace_back(
            std::make_unique<FramePass<PassData>>(std::move(name),
                                                  id,
                                                  std::forward<SetupCallback>(setup_callback),
                                                  std::forward<ExecuteCallback>(execute_callback),
                                                  type));

        auto builder = FramePassBuilder { *this, frame_pass };
        frame_pass.setup(builder);

        return static_cast<FramePass<PassData> &>(frame_pass);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    FramePassResourceID<Resource>
        FrameGraph::setRetainedResource(std::string name,
                                        typename Resource::Descriptor descriptor,
                                        Resource &&resource) {
        const auto id = m_resource_id++;

        auto fp_resource =
            std::make_unique<FramePassResource<Resource>>(std::move(name),
                                                          id,
                                                          std::move(descriptor),
                                                          std::forward<Resource>(resource));
        return FramePassResourceID<Resource> { create(std::move(fp_resource)) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    FramePassResourceID<Resource>
        FrameGraph::createTransientResource(std::string name,
                                            typename Resource::Descriptor descriptor) {
        const auto id = m_resource_id++;

        auto resource = std::make_unique<FramePassResource<Resource>>(std::move(name),
                                                                      id,
                                                                      std::move(descriptor));
        return FramePassResourceID<Resource> { create(std::move(resource)) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    const Resource &FrameGraph::get(FramePassResourceID<Resource> id) const noexcept {
        return getResource(id).resource();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    const FramePassResource<Resource> &
        FrameGraph::getResource(FramePassResourceID<Resource> id) const noexcept {
        return static_cast<const FramePassResource<Resource> &>(getResourceBase(id));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    FramePassResource<Resource> &
        FrameGraph::getResource(FramePassResourceID<Resource> id) noexcept {
        return static_cast<FramePassResource<Resource> &>(getResourceBase(id));
    }
} // namespace storm::engine
