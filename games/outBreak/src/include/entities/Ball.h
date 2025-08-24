#pragma once

#include <Engine.h>

using namespace engine;

namespace outBreak {
    class Ball : public Entity {
        public:
            Ball() = default;
            virtual ~Ball() = default;

            void init() override;
            void update(float deltaTime) override;
            void render() override;

            void setVelocity(const Vector2& velocity) { 
                m_Direction = velocity.normalize(); 
            }
            Vector2 getVelocity() const { return m_Direction * BALL_SPEED; }

            void reverseVelocityX() { m_Direction = Vector2(-m_Direction.getRawX(), m_Direction.getRawY()); }
            void reverseVelocityY() { m_Direction = Vector2(m_Direction.getRawX(), -m_Direction.getRawY()); }
            
            // Reset to predictable 45-degree angle using engine's normalize()
            void resetDirection() { 
                m_Direction = Vector2{1.0f, -1.0f}.normalize(); 
            }

        private:
            Vector2 m_Direction{ Vector2{1.0f, -1.0f}.normalize() }; // normalized direction vector
            static constexpr float BALL_SIZE = 16.0f;
            static constexpr float BALL_SPEED = 300.0f; // pixels per second - constant!
    };
}
