#pragma once

#include <nlohmann/json.hpp>
#include <SDL3/SDL.h>

#include <unordered_map>
#include <iostream>
#include <fstream>

using json = nlohmann::json;

namespace crimson {
    struct Vector2 {
        Vector2 operator+(const Vector2& other) const { return { x + other.x, y + other.y }; }
        Vector2 operator*(float scalar) const { return { x * scalar, y * scalar }; }

        Vector2 normalize() const;

        float x = 0;
        float y = 0;
    };

    class InputAction {
        public:
            virtual ~InputAction() = default;

            virtual bool isPressed() const = 0;
            virtual Vector2 getDirection() const { return { 0, 0 }; }
    };

    class DirectionalInputAction : public InputAction {
        public:
            DirectionalInputAction(SDL_Keycode key, Vector2 direction) : m_Key(key), m_Direction(direction) {}

            bool isPressed() const override;
            Vector2 getDirection() const override { return m_Direction; }

            SDL_Keycode m_Key;
            Vector2 m_Direction;
    };

    class SimpleInputAction : public InputAction {
        public:
            SimpleInputAction(SDL_Keycode key) : m_Key(key) {}
            bool isPressed() const override;

            SDL_Keycode m_Key;
    };

    class InputSystem {
        public:
            void addMovementAction(const std::string& name, SDL_Keycode key, const Vector2& m_Direction) { m_Actions[name] = std::make_shared<DirectionalInputAction>(key, m_Direction); }
            void addSimpleAction(const std::string& name, SDL_Keycode key) { m_Actions[name] = std::make_shared<SimpleInputAction>(key); }
            void loadInputActions(const std::string& filename);

            bool isActionPressed(const std::string& actionName);

            Vector2 getMovementVector() const;

            std::unordered_map<std::string, std::shared_ptr<InputAction>> m_Actions;
    };
}
