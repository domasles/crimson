#include <Input.h>

namespace crimson {
    Vector2 Vector2::Normalize() const {
        float length = SDL_sqrtf(x * x + y * y);
        return (length > 0) ? Vector2{ x / length, y / length } : Vector2{ 0, 0 };
    }

    bool DirectionalInputAction::IsPressed() const {
        const bool* state = SDL_GetKeyboardState(nullptr);
        return state[SDL_GetScancodeFromKey(m_Key, nullptr)] != 0;
    }

    bool SimpleInputAction::IsPressed() const {
        const bool* state = SDL_GetKeyboardState(nullptr);
        return state[SDL_GetScancodeFromKey(m_Key, nullptr)] != 0;
    }

    Vector2 InputSystem::GetMovementVector() {
        Vector2 movement = { 0, 0 };

        for (const auto& [name, action] : actions) {
            if (action->IsPressed()) {
                movement = movement + action->GetDirection();
            }
        }

        return movement.Normalize();
    }

    bool InputSystem::IsActionPressed(const std::string& actionName) {
        if (actions.count(actionName)) {
            return actions[actionName]->IsPressed();
        }

        return false;
    }

    void InputSystem::LoadInputActions(const std::string& filename) {
        std::ifstream file(filename);

        if (!file.is_open()) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open input actions file!");
            return;
        }

        json inputJson;
        file >> inputJson;

        for (const auto& [name, data] : inputJson["actions"].items()) {
            std::string type = data["type"];

            SDL_Keycode key = SDL_GetKeyFromName(data["key"].get<std::string>().c_str());

            if (key == SDLK_UNKNOWN) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unknown key: %s", data["key"].get<std::string>().c_str());
                continue;
            }

            if (type == "directional") {
                Vector2 direction = { data["vector"][0], data["vector"][1] };
                AddDirectionalAction(name, key, direction);
            }

            else if (type == "simple") {
                AddSimpleAction(name, key);
            }
        }
    }

}
