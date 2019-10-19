// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    template<typename PassData>
    FramePass<PassData> &
        FrameGraph::addPass(std::string name,
                            typename FramePass<PassData>::SetupCallback &&setup_callback,
                            typename FramePass<PassData>::ExecuteCallback &&execute_callback,
                            bool secondary_command_buffer) {
        using SetupCallback   = typename FramePass<PassData>::SetupCallback;
        using ExecuteCallback = typename FramePass<PassData>::ExecuteCallback;

        auto &frame_pass = *m_frame_passes.emplace_back(
            std::make_unique<FramePass<PassData>>(std::move(name),
                                                  std::forward<SetupCallback>(setup_callback),
                                                  std::forward<ExecuteCallback>(execute_callback),
                                                  secondary_command_buffer));

        auto builder = FramePassBuilder { *this, frame_pass };
        frame_pass.setup(builder);

        return static_cast<FramePass<PassData> &>(frame_pass);
    }
} // namespace storm::engine
