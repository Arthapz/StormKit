#pragma once

#include <storm/entities/Component.hpp>
#include <storm/core/Math.hpp>

namespace storm::graphics {
	struct STORM_PUBLIC CameraComponent : public entities::Component {
		explicit CameraComponent(){};
		~CameraComponent() override {
		}

		CameraComponent(const CameraComponent &);
		CameraComponent &operator=(const CameraComponent &);

		CameraComponent(CameraComponent &&);
		CameraComponent &operator=(CameraComponent &&);

		core::Matrix projection_matrix;
		core::Matrix view_matrix;

		static constexpr Type TYPE = "CameraComponent"_component_type;
	};
} // namespace storm::graphics
