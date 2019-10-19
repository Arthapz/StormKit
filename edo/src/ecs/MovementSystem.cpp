#include "MovementSystem.hpp"
#include "TransformComponent.hpp"

#include <storm/log/LogHandler.hpp>

#include <storm/entities/EntityManager.hpp>
#include <storm/entities/MessageBus.hpp>

////////////////////////////////////////
////////////////////////////////////////
MovementSystem::MovementSystem(storm::entities::EntityManager &manager)
	: storm::entities::System{manager, 1u, {TransformComponent::TYPE}} {
}

////////////////////////////////////////
////////////////////////////////////////
MovementSystem::~MovementSystem() = default;

////////////////////////////////////////
////////////////////////////////////////
MovementSystem::MovementSystem(MovementSystem &&) = default;

////////////////////////////////////////
////////////////////////////////////////
MovementSystem &MovementSystem::operator=(MovementSystem &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void MovementSystem::update(std::uint64_t delta) {
	using storm::log::operator""_module;
	static constexpr auto velocity = 200.f / 1000000.f;

	const auto current_velocity = velocity * gsl::narrow_cast<float>(delta);
	for (auto entity : m_entities) {
		auto &transform_component =
			m_manager.get().getComponent<TransformComponent>(entity);

		if (m_input_state.up) {
			transform_component.transform_matrix =
				storm::core::translate(transform_component.transform_matrix,
									   {0.f, -current_velocity, 0.f});
		}
		if (m_input_state.down) {
			transform_component.transform_matrix =
				storm::core::translate(transform_component.transform_matrix,
									   {0.f, current_velocity, 0.f});
		}
		if (m_input_state.left) {
			transform_component.transform_matrix =
				storm::core::translate(transform_component.transform_matrix,
									   {-current_velocity, 0.f, 0.f});
		}
		if (m_input_state.right) {
			transform_component.transform_matrix =
				storm::core::translate(transform_component.transform_matrix,
									   {current_velocity, 0.f, 0.f});
		}
	}
}

void MovementSystem::onMessageReceived([
	[maybe_unused]] const storm::entities::Message &message) {
	return;
}
