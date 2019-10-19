#include <storm/render/core/Types.hpp>

using namespace storm;
using namespace storm::render;

namespace std {
    core::Hash64 hash<Viewport>::operator()(const Viewport &viewport) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hash_combine(hash, viewport.depth);
        core::hash_combine(hash, viewport.extent);
        core::hash_combine(hash, viewport.position);

        return hash;
    }

    core::Hash64 hash<Scissor>::operator()(const Scissor &scissor) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hash_combine(hash, scissor.offset);
        core::hash_combine(hash, scissor.extent);

        return hash;
    }
} // namespace std
