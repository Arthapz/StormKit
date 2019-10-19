// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Platform.hpp>

/////////// - StormKit::entities - ///////////
#include <storm/entities/Component.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    struct TransformComponentCache {
        render::DescriptorSetOwnedPtr descriptor_set;
    };
    struct STORM_PUBLIC TransformComponent: public entities::Component {
        TransformOwnedPtr transform;

        std::unique_ptr<TransformComponentCache> cache;

        static constexpr Type TYPE = "TransformComponent"_component_type;
    };
} // namespace storm::engine