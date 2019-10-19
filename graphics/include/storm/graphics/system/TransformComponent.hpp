#pragma once

#include <storm/entities/Component.hpp>
#include <storm/core/Math.hpp>

namespace storm::graphics {
	struct STORM_PUBLIC TransformComponent : public entities::Component {
		explicit TransformComponent(){};
		~TransformComponent() override {
		}

		TransformComponent(const TransformComponent &);
		TransformComponent &operator=(const TransformComponent &);

		TransformComponent(TransformComponent &&);
		TransformComponent &operator=(TransformComponent &&);

		core::Matrix transform_matrix;

		static constexpr Type TYPE = "TransformComponent"_component_type;
	};
} // namespace storm::graphics
