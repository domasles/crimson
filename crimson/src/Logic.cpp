#include <Logic.h>

namespace crimson {
    void Logic::init() {
        m_InputSystem.LoadInputActions("config/InputActions.json");
    }

    void Logic::handleInput() {
        Vector2 movement = m_InputSystem.GetMovementVector();
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Current movement input vector: (%.2f, %.2f)", movement.x, movement.y);

        if (m_InputSystem.IsActionPressed("shoot")) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Shooting!");
        }
    }
}
