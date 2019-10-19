#pragma once

#include <filesystem>

#include <storm/core/Platform.hpp>

#include <storm/graphics/core/Vertex.hpp>

#ifdef STORM_OS_MACOS
static const auto RESOURCE_DIR =
        std::filesystem::current_path() / "../Resources/";
#else
static const auto RESOURCE_DIR = std::filesystem::current_path();
#endif

using Vertex = storm::graphics::VertexP2UV2;
