#pragma once

#include <array>

#include <storm/core/Span.hpp>

namespace storm::graphics {
    static constexpr auto QUAD_VERTEX_SHADER_SPV = std::array{
#include "shaders/QuadVertex.hpp"
    };

    static const auto QUAD_VERTEX_SHADER = core::span<const std::byte>{
        reinterpret_cast<const std::byte *>(std::data(QUAD_VERTEX_SHADER_SPV)),
        std::size(QUAD_VERTEX_SHADER_SPV) * sizeof(int)};

    static constexpr auto QUAD_FRAGMENT_SHADER_SPV = std::array{
#include "shaders/QuadFragment.hpp"
    };
    static const auto QUAD_FRAGMENT_SHADER = core::span<const std::byte>{
        reinterpret_cast<const std::byte *>(
            std::data(QUAD_FRAGMENT_SHADER_SPV)),
        std::size(QUAD_FRAGMENT_SHADER_SPV) * sizeof(int)};
} // namespace storm::graphics
