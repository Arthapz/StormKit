#include <storm/core/Math.hpp>

using namespace storm;
using namespace storm::core;

namespace std {
	std::size_t hash<Extent>::operator()(const Extent &extent) const noexcept {
		auto hash = std::size_t{0};
		core::hash_combine(hash, extent.w);
		core::hash_combine(hash, extent.h);

		return hash;
	}

	std::size_t hash<Extentf>::operator()(const Extentf &extent) const
		noexcept {
		auto hash = std::size_t{0};
		core::hash_combine(hash, extent.w);
		core::hash_combine(hash, extent.h);

		return hash;
	}
} // namespace std
