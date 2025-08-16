#pragma once

#include <utils/math.h>

using namespace engine::utils::math;

namespace engine {
    class Transform {
        public:
            Transform(const Vector2& position, const Vector2& size = {100.0f, 100.0f}, float rotation = 0.0f) : m_Position(position), m_Size(size), m_Rotation(rotation) {}

            Transform() = default;
            ~Transform() = default;

            void setPosition(const Vector2& position) { m_Position = position; }
            void addPosition(const Vector2& offset) { m_Position += offset; }

            void setSize(const Vector2& size) { m_Size = size; }
            void setRotation(float rotation) { m_Rotation = rotation; }
            void addRotation(float delta) { m_Rotation += delta; }

            void setScale(const Vector2& scale) { m_Scale = scale; }
            void setScale(float uniformScale) { m_Scale = Vector2(uniformScale, uniformScale); }

            const Vector2& getPosition() const { return m_Position; }
            const Vector2& getSize() const { return m_Size; }
            const Vector2& getScale() const { return m_Scale; }

            float getRotation() const { return m_Rotation; }

            Vector2 getWorldPosition() const { return m_Position; }
            Vector2 getWorldSize() const { return m_Size * m_Scale; }
            Vector2 getCenter() const { return m_Position + (getWorldSize() / 2.0f); }
            
            void lookAt(const Vector2& target) {
                Vector2 direction = target - m_Position;
                m_Rotation = atan2(direction.getRawY(), direction.getRawX()) * (180.0f / 3.14159265359f);
            }

        private:
            Vector2 m_Position{0.0f, 0.0f};
            Vector2 m_Size{100.0f, 100.0f};
            Vector2 m_Scale{1.0f, 1.0f};

            float m_Rotation = 0.0f;
    };
}
