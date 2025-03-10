#pragma once

#include <utils/math.h>

using namespace crimson::utils::math;
using json = nlohmann::json;

namespace crimson {

    class InputAction {
        public:
            virtual ~InputAction() = default;

            virtual bool isPressed() const = 0;
            virtual Vector2 getDirection() const { return { 0, 0 }; }
    };

    class DirectionalInputAction : public InputAction {
        public:
            DirectionalInputAction(const SDL_Keycode key, const Vector2 direction) : m_Key(key), m_Direction(direction) {}

            bool isPressed() const override;

            Vector2 getDirection() const override { return m_Direction; }

        private:
            SDL_Keycode m_Key;
            Vector2 m_Direction;
    };

    class SimpleInputAction : public InputAction {
        public:
            SimpleInputAction(const SDL_Keycode key) : m_Key(key) {}
            bool isPressed() const override;

        private:
            SDL_Keycode m_Key;
    };

    class InputSystem {
        public:
            void addMovementAction(const std::string& name, const SDL_Keycode key, const Vector2& m_Direction);
            void addSimpleAction(const std::string& name, const SDL_Keycode key);
            void loadInputActions(const std::string& fileName);

            bool isActionPressed(const std::string& actionName) const;

            Vector2 getMovementVector() const;

        private:
            bool m_IsLoaded = false;
            std::unordered_map<std::string, std::shared_ptr<InputAction>> m_Actions;
    };
}
