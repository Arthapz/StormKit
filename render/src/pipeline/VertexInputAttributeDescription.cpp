#include <storm/render/pipeline/VertexInputAttributeDescription.hpp>

using namespace storm;
using namespace storm::render;

namespace std {
    core::Hash64 hash<VertexInputAttributeDescription>::operator()(
        const VertexInputAttributeDescription &description) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hash_combine(hash, description.binding);
        core::hash_combine(hash, description.location);
        core::hash_combine(hash, description.format);
        core::hash_combine(hash, description.offset);

        return hash;
    }

    core::Hash64 hash<storm::render::VertexInputAttributeDescriptionArray>::operator()(
        const storm::render::VertexInputAttributeDescriptionArray &bindings) const noexcept {
        auto hash = core::Hash64 { 0 };
        for (const auto &binding : bindings) core::hash_combine(hash, binding);

        return hash;
    }

    core::Hash64 hash<storm::render::VertexInputAttributeDescriptionSpan>::operator()(
        const storm::render::VertexInputAttributeDescriptionSpan &bindings) const noexcept {
        auto hash = core::Hash64 { 0 };
        for (const auto &binding : bindings) core::hash_combine(hash, binding);

        return hash;
    }

    core::Hash64 hash<storm::render::VertexInputAttributeDescriptionConstSpan>::operator()(
        const storm::render::VertexInputAttributeDescriptionConstSpan &bindings) const noexcept {
        auto hash = core::Hash64 { 0 };
        for (const auto &binding : bindings) core::hash_combine(hash, binding);

        return hash;
    }
} // namespace std
