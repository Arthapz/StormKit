#pragma once

#include <storm/core/Memory.hpp>

#include <storm/entities/System.hpp>

class MovementSystem : public storm::entities::System {
  public:
    struct InputState {
        bool up	= false;
        bool left  = false;
        bool right = false;
        bool down  = false;
    };

    explicit MovementSystem(storm::entities::EntityManager &manager);
    ~MovementSystem() override;

    MovementSystem(MovementSystem &&);
    MovementSystem &operator=(MovementSystem &&);

    inline void updateInputState(InputState &&input_state) {
        m_input_state = std::move(input_state);
    }
    void update(std::uint64_t delta) override;

  protected:
    void onMessageReceived(const storm::entities::Message &message) override;

  private:
    InputState m_input_state;
};

DECLARE_PTR_AND_REF(MovementSystem)
