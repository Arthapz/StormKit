#include <storm/render/pipeline/VertexBindingDescription.hpp>

using namespace storm;
using namespace storm::render;

namespace std {
	std::size_t hash<VertexBindingDescription>::
		operator()(const VertexBindingDescription &description) const noexcept {
		auto hash = std::size_t{0};
		core::hash_combine(hash, description.binding);
		core::hash_combine(hash, description.stride);
		core::hash_combine(hash, description.input_rate);

		return hash;
	}

	std::size_t hash<storm::render::VertexBindingDescriptions>::operator()(
		const storm::render::VertexBindingDescriptions &bindings) const
		noexcept {
		auto hash = std::size_t{0};
		for (const auto &binding : bindings) core::hash_combine(hash, binding);

		return hash;
	}
} // namespace std
