#pragma once

#include <storm/entities/Component.hpp>

#include <storm/render/resource/Fwd.hpp>
#include <storm/render/resource/HardwareBuffer.hpp>

namespace storm::graphics {
	struct STORM_PUBLIC StaticGeometryComponent : public entities::Component {
		explicit StaticGeometryComponent() {
		}
		~StaticGeometryComponent() override {
		}

		StaticGeometryComponent(const StaticGeometryComponent &);
		StaticGeometryComponent &operator=(const StaticGeometryComponent &);

		StaticGeometryComponent(StaticGeometryComponent &&);
		StaticGeometryComponent &operator=(StaticGeometryComponent &&);

		std::uint32_t vertex_count = 0;
		std::uint32_t index_count  = 0;

		render::HardwareBufferOwnedPtr vertex_buffer;
		render::HardwareBufferOwnedPtr index_buffer;

		static constexpr Type TYPE = "StaticGeometryComponent"_component_type;
	};
} // namespace storm::graphics
