#include <storm/render/pipeline/VertexInputAttributeDescription.hpp>

using namespace storm;
using namespace storm::render;

namespace std {
	std::size_t hash<VertexInputAttributeDescription>::
		operator()(const VertexInputAttributeDescription &description) const
		noexcept {
		auto hash = std::size_t{0};
		core::hash_combine(hash, description.binding);
		core::hash_combine(hash, description.location);
		core::hash_combine(hash, description.format);
		core::hash_combine(hash, description.offset);

		return hash;
	}

	std::size_t hash<storm::render::VertexInputAttributeDescriptions>::
		operator()(const storm::render::VertexInputAttributeDescriptions
					   &bindings) const noexcept {
		auto hash = std::size_t{0};
		for (const auto &binding : bindings) core::hash_combine(hash, binding);

		return hash;
	}
} // namespace std
