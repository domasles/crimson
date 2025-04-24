#pragma once

#include <utils/math.h>

using namespace engine::utils::math;
using json = nlohmann::json;

namespace engine {
    class InputAction {
        public:
            InputAction(const SDL_Keycode key) : m_Key(key) {}

            virtual ~InputAction() = default;
            virtual const Vector2 getDirection() const { return { 0, 0 }; }

            const bool isPressed() const;

        protected:
            SDL_Keycode m_Key;
    };

    class DirectionalInputAction : public InputAction {
        public:
            DirectionalInputAction(const SDL_Keycode key, const Vector2 direction) : InputAction(key), m_Direction(direction) {}
            const Vector2 getDirection() const override { return m_Direction; }

        private:
            Vector2 m_Direction;
    };

    class SimpleInputAction : public InputAction {
        public:
            SimpleInputAction(const SDL_Keycode key) : InputAction(key) {}
    };

    class InputSystem {
        public:
            InputSystem(const std::string& workingDir) : m_WorkingDir(workingDir) {}
            ~InputSystem() = default;

            void addMovementAction(const std::string& name, const SDL_Keycode key, const Vector2& m_Direction);
            void addSimpleAction(const std::string& name, const SDL_Keycode key);

            const bool loadInputActions(const std::string& fileName);
            const bool isActionPressed(const std::string& actionName) const;

            const Vector2 getMovementVector() const;

        private:
            bool m_IsLoaded = false;

            json m_JsonFile;

            std::string m_WorkingDir;
            std::unordered_map<std::string, std::shared_ptr<InputAction>> m_Actions;
    };
}
