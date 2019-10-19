// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <unordered_set>

/////////// - StormKit::core - ///////////
#include <storm/core/Platform.hpp>

/////////// - StormKit::entities - ///////////
#include <storm/entities/Component.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    struct STORM_PUBLIC StaticMeshComponent: public entities::Component {
        StaticMeshOwnedPtr mesh;

        std::unordered_set<std::string> passes_to_render_in;

        static constexpr Type TYPE = "StaticMeshComponent"_component_type;
    };
} // namespace storm::engine