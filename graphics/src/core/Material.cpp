#include <storm/graphics/core/Material.hpp>

using namespace storm;
using namespace storm::graphics;

namespace std {
	std::size_t hash<Material>::operator()(const Material &material) const
		noexcept {
        static auto texture_ptr_hasher =
            std::hash<const storm::render::Texture *>{};
        static auto sampler_ptr_hasher =
            std::hash<const storm::render::Sampler *>{};

        auto hash = texture_ptr_hasher(material.diffuse_map.get());
        core::hash_combine(
            hash, sampler_ptr_hasher(material.diffuse_map_sampler.get()));

		return hash;
	}
} // namespace std
