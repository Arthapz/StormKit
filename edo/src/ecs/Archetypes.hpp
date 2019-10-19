#pragma once

#include <storm/render/core/Fwd.hpp>
#include <storm/core/Math.hpp>
#include <storm/render/pipeline/Fwd.hpp>
#include <storm/render/resource/Fwd.hpp>

#include <storm/entities/Entity.hpp>
#include <storm/entities/Fwd.hpp>

storm::entities::Entity makeCamera(storm::entities::EntityManager &manager,
                                   storm::core::Extentf size);

storm::entities::Entity
    makeSprite(const storm::render::Device &device,
               const storm::render::GraphicsPipelineState &pipeline,
               storm::entities::EntityManager &manager,
               storm::core::Extentf size,
               const storm::render::Texture &texture,
               const storm::render::Sampler &sampler);

storm::entities::Entity
    makeMap(const storm::render::Device &device,
            const storm::render::GraphicsPipelineState &pipeline,
            storm::entities::EntityManager &manager, storm::core::Extent size,
            const storm::render::Texture &texture,
            const storm::render::Sampler &sampler,
            const storm::render::DescriptorSet &set);
