// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    const render::DescriptorSetLayout &PipelineBuilder::cameraLayout() const noexcept {
        return *m_camera_layout;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const render::DescriptorSetLayout &PipelineBuilder::transformLayout() const noexcept {
        return *m_transform_layout;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const render::DescriptorSetLayout &PipelineBuilder::pbrMaterialLayout() const noexcept {
        return *m_pbr_material_layout;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const render::GraphicsPipelineLayout &PipelineBuilder::pbrPipelineLayout() const noexcept {
        return m_pbr_pipeline_layout;
    }
} // namespace storm::engine
