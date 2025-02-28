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

        Vector2 Normalize() const;

        float x = 0;
        float y = 0;
    };

    class InputAction {
        public:
            virtual ~InputAction() = default;

            virtual bool IsPressed() const = 0;
            virtual Vector2 GetDirection() const { return { 0, 0 }; }
    };

    class DirectionalInputAction : public InputAction {
        public:
            DirectionalInputAction(SDL_Keycode key, Vector2 direction) : m_Key(key), m_Direction(direction) {}

            bool IsPressed() const override;
            Vector2 GetDirection() const override { return m_Direction; }

            SDL_Keycode m_Key;
            Vector2 m_Direction;
    };

    class SimpleInputAction : public InputAction {
        public:
            SimpleInputAction(SDL_Keycode key) : m_Key(key) {}
            bool IsPressed() const override;

            SDL_Keycode m_Key;
    };

    class InputSystem {
        public:
            void AddDirectionalAction(const std::string& name, SDL_Keycode key, const Vector2& m_Direction) { actions[name] = std::make_shared<DirectionalInputAction>(key, m_Direction); }
            void AddSimpleAction(const std::string& name, SDL_Keycode key) { actions[name] = std::make_shared<SimpleInputAction>(key); }
            void LoadInputActions(const std::string& filename);

            bool IsActionPressed(const std::string& actionName);

            Vector2 GetMovementVector();

            std::unordered_map<std::string, std::shared_ptr<InputAction>> actions;
    };
}
