// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "FramePassBuilder.hpp"

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    [[nodiscard]] FramePassResourceID<T>
        FramePassBuilder::create(std::string name, typename T::Descriptor descriptor) {
        auto id = m_frame_graph->createTransientResource<T>(std::move(name), std::move(descriptor));

        auto &resource        = m_frame_graph->getResource(id);
        resource.m_created_by = m_frame_pass->id();

        m_frame_pass->m_creates.emplace_back(id);

        return id;
    }
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    [[nodiscard]] FramePassResourceID<T> FramePassBuilder::read(FramePassResourceID<T> input) {
        auto &resource = m_frame_graph->getResource(input);
        resource.m_read_in.emplace_back(m_frame_pass->id());

        m_frame_pass->m_reads.emplace_back(input);

        return input;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    [[nodiscard]] FramePassResourceID<T> FramePassBuilder::write(FramePassResourceID<T> input) {
        auto &resource = m_frame_graph->getResource(input);
        resource.m_write_in.emplace_back(m_frame_pass->id());

        m_frame_pass->m_writes.emplace_back(input);

        return input;
    }
} // namespace storm::engine
