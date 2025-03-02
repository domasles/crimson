#include <Input.h>

namespace crimson {
    Vector2 Vector2::normalize() const {
        float length = SDL_sqrtf(x * x + y * y);
        return (length > 0) ? Vector2{ x / length, y / length } : Vector2{ 0, 0 };
    }

    bool DirectionalInputAction::isPressed() const {
        const bool* state = SDL_GetKeyboardState(nullptr);
        return state[SDL_GetScancodeFromKey(m_Key, nullptr)] != 0;
    }

    bool SimpleInputAction::isPressed() const {
        const bool* state = SDL_GetKeyboardState(nullptr);
        return state[SDL_GetScancodeFromKey(m_Key, nullptr)] != 0;
    }

    Vector2 InputSystem::getMovementVector() const {
        Vector2 movement = { 0, 0 };

        for (const auto& [name, action] : m_Actions) {
            if (action->isPressed()) {
                movement = movement + action->getDirection();
            }
        }

        return movement.normalize();
    }

    bool InputSystem::isActionPressed(const std::string& actionName) {
        if (m_Actions.count(actionName)) {
            return m_Actions[actionName]->isPressed();
        }

        return false;
    }

    void InputSystem::loadInputActions(const std::string& fileName) {
        std::string basePath = SDL_GetBasePath();
        std::string fullPath = basePath + fileName;

        std::ifstream file(fullPath);

        if (!file.is_open()) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open input actions file!");
            return;
        }

        json inputJson;

        try {
            file >> inputJson;
        }

        catch (const std::exception& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
        }

        for (const auto& [name, data] : inputJson["actions"].items()) {
            std::string type = data["type"];

            SDL_Keycode key = SDL_GetKeyFromName(data["key"].get<std::string>().c_str());

            if (key == SDLK_UNKNOWN) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unknown key: %s", data["key"].get<std::string>().c_str());
                continue;
            }

            if (type == "movement") {
                Vector2 direction = { data["vector"][0], data["vector"][1] };
                addMovementAction(name, key, direction);
            }

            else if (type == "simple") {
                addSimpleAction(name, key);
            }
        }
    }

}
