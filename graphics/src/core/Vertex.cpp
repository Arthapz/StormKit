#include <storm/graphics/core/Vertex.hpp>

namespace storm::graphics {
	const render::VertexInputAttributeDescriptions
		VertexP2C3::ATTRIBUTE_DESCRIPTIONS = {
			{0, 0, render::Format::Float2, offsetof(VertexP2C3, position)},
            {0, 1, render::Format::Float3, offsetof(VertexP2C3, color)}};

    const render::VertexInputAttributeDescriptions
        VertexP2UV2::ATTRIBUTE_DESCRIPTIONS = {
            {0, 0, render::Format::Float2, offsetof(VertexP2C3, position)},
            {0, 1, render::Format::Float2, offsetof(VertexP2UV2, uv)}};

	const render::VertexInputAttributeDescriptions
		VertexP3C3::ATTRIBUTE_DESCRIPTIONS = {
			{0, 0, render::Format::Float3, offsetof(VertexP3C3, position)},
			{0, 1, render::Format::Float3, offsetof(VertexP3C3, color)}};

	const render::VertexInputAttributeDescriptions
		VertexP3N3::ATTRIBUTE_DESCRIPTIONS = {
			{0, 0, render::Format::Float3, offsetof(VertexP3N3, position)},
			{0, 1, render::Format::Float3, offsetof(VertexP3N3, normal)}};

	const render::VertexInputAttributeDescriptions
		VertexP3C3N3::ATTRIBUTE_DESCRIPTIONS = {
			{0, 0, render::Format::Float3, offsetof(VertexP3C3N3, position)},
			{0, 1, render::Format::Float3, offsetof(VertexP3C3N3, color)},
			{0, 2, render::Format::Float3, offsetof(VertexP3C3N3, normal)}};

	const render::VertexInputAttributeDescriptions
		VertexP3C3N3UV2::ATTRIBUTE_DESCRIPTIONS = {
			{0, 0, render::Format::Float3, offsetof(VertexP3C3N3UV2, position)},
			{0, 1, render::Format::Float3, offsetof(VertexP3C3N3UV2, color)},
			{0, 2, render::Format::Float3, offsetof(VertexP3C3N3UV2, normal)},
			{0, 3, render::Format::Float2, offsetof(VertexP3C3N3UV2, uv)}};
} // namespace storm::graphics
