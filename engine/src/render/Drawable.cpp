/////////// - StormKit::engine - ///////////
#include <storm/engine/render/Drawable.hpp>

using namespace storm;
using namespace storm::engine;

namespace std {
    core::Hash64 hash<Drawable>::operator()(const Drawable &drawable) const noexcept {
        auto hash = core::Hash64 { 0 };

        core::hashCombine(hash, drawable.vertices());
        core::hashCombine(hash, drawable.indices());

        return hash;
    }
} // namespace std

////////////////////////////////////////
////////////////////////////////////////
Drawable::Drawable() noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
Drawable::~Drawable() = default;

////////////////////////////////////////
////////////////////////////////////////
Drawable::Drawable(Drawable &&) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
auto Drawable::operator=(Drawable &&) noexcept -> Drawable& = default;
