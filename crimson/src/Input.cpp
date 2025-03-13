#include <pch.h>

#include <Input.h>

namespace crimson {
    const bool InputAction::isPressed() const {
        const bool* state = SDL_GetKeyboardState(nullptr);

        SDL_Scancode scancode = SDL_GetScancodeFromKey(m_Key, nullptr);

        if (scancode == SDL_SCANCODE_UNKNOWN) {
            return false;
        }

        return state[scancode] != 0;
    }

    void InputSystem::addMovementAction(const std::string& name, const SDL_Keycode key, const Vector2& m_Direction) {
        m_Actions[name] = std::make_shared<DirectionalInputAction>(key, m_Direction);
    }

    void InputSystem::addSimpleAction(const std::string& name, const SDL_Keycode key) {
        m_Actions[name] = std::make_shared<SimpleInputAction>(key);
    }

    void InputSystem::loadInputActions(const std::string& fileName) {
        std::string filePath = std::string(SDL_GetBasePath()) + fileName;
        std::ifstream file(filePath);

        if (!file) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open %s!", fileName.c_str());
            return;
        }

        if (m_IsLoaded) {
            m_Actions.clear();
        }

        m_IsLoaded = true;

        try {
            json inputJson;
            file >> inputJson;

            if (!inputJson.contains("actions")) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Missing 'actions' field in JSON.");
                return;
            }

            for (const auto& [name, data] : inputJson["actions"].items()) {
                if (!data.contains("type") || !data.contains("key")) {
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Malformed action data: missing 'type' or 'key'.");
                    continue;
                }

                const std::string& type = data["type"];

                SDL_Keycode key = SDL_GetKeyFromName(data["key"].get<std::string>().c_str());

                if (key == SDLK_UNKNOWN) {
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unknown key: %s", data["key"].get<std::string>().c_str());
                    continue;
                }

                if (type == "movement") {
                    if (!data.contains("vector") || data["vector"].size() < 2) {
                        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Malformed movement action: missing 'vector'.");
                        continue;
                    }

                    addMovementAction(name, key, { data["vector"][0], data["vector"][1] });
                }

                else if (type == "simple") {
                    addSimpleAction(name, key);
                }

                else {
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unknown action type: %s", type.c_str());
                }
            }
        }

        catch (const std::exception& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSON parsing error: %s", e.what());
        }
    }

    const bool InputSystem::isActionPressed(const std::string& actionName) const {
        auto it = m_Actions.find(actionName);

        if (it != m_Actions.end()) {
            return it->second->isPressed();
        }

        return false;
    }

    const Vector2 InputSystem::getMovementVector() const {
        Vector2 movement = { 0, 0 };

        for (const auto& [name, action] : m_Actions) {
            if (action->isPressed()) {
                movement += action->getDirection();
            }
        }

        return movement.normalize();
    }
}
