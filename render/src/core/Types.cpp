#include <storm/render/core/Types.hpp>

using namespace storm;
using namespace storm::render;

namespace std {
	std::size_t hash<Viewport>::operator()(const Viewport &viewport) const
		noexcept {
		auto hash = std::size_t{0};
		core::hash_combine(hash, viewport.depth);
		core::hash_combine(hash, viewport.extent);
		core::hash_combine(hash, viewport.position);

		return hash;
	}

	std::size_t hash<Scissor>::operator()(const Scissor &scissor) const
		noexcept {
		auto hash = std::size_t{0};
		core::hash_combine(hash, scissor.offset);
		core::hash_combine(hash, scissor.extent);

		return hash;
	}
} // namespace std
