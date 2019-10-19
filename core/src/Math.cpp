#include <storm/core/Math.hpp>

using namespace storm;
using namespace storm::core;

namespace std {
    auto hash<Extentu>::operator()(const Extentu &extent) const noexcept -> Hash64 {
        auto hash = Hash64 { 0 };
        hash_combine(hash, extent.w);
        hash_combine(hash, extent.h);

        return hash;
    }

    auto hash<Extentf>::operator()(const Extentf &extent) const noexcept -> Hash64 {
        auto hash = Hash64 { 0 };
        hash_combine(hash, extent.w);
        hash_combine(hash, extent.h);

        return hash;
    }
} // namespace std
