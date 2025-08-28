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

            void setVelocity(const Vector2& velocity) { m_Direction = velocity.normalize(); }

            void setDirectionX(float sign);
            void setDirectionY(float sign);

            void resetDirection() { m_Direction = Vector2{1.0f, -1.0f}.normalize(); }
            void resetPosition();

            Vector2 getInitialPosition() const { return m_InitialPosition; }
            Vector2 getVelocity() const { return m_Direction * BALL_SPEED; }

        private:
            static constexpr float BALL_SIZE = 16.0f;
            static constexpr float BALL_SPEED = 300.0f;

            Vector2 m_InitialPosition{ getLogicalWindowSize().getRawX()/2 - BALL_SIZE/2, 700.0f };
            Vector2 m_Direction{ Vector2{1.0f, -1.0f}.normalize() };
            Vector2 m_LocalOffset{ 0.0f, 0.0f };
    };
}
