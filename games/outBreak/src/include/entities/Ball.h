#pragma once

#include <Engine.h>

using namespace engine;

namespace outBreak {
    class Brick;
    class Ball : public Entity {
        public:
            Ball() = default;
            virtual ~Ball() = default;

            void init() override;
            void update(float deltaTime) override;
            void render() override;

            void resetDirection();
            void resetPosition();

            Vector2 getInitialPosition() const { return m_InitialPosition; }
            Vector2 getVelocity() const { return m_Direction * BALL_SPEED; }

        private:
            bool handleWallCollision(Vector2& nextPos);
            void handleCollisions(Vector2& nextPos);
            void playImpactSound();
            void reflectDirection(const Vector2& normal);

            static constexpr float BALL_SIZE = 24.0f;
            static constexpr float BALL_SPEED = 500.0f;
            static constexpr float BALL_ROTATION_SPEED = 10.0f;

            float m_RotationSpeed = 0.0f;

            Vector2 m_InitialPosition{ getLogicalWindowSize().getRawX()/2 - BALL_SIZE/2, 700.0f };
            Vector2 m_Direction{ Vector2{1.0f, -1.0f}.normalize() };
            Vector2 m_LocalOffset{ 0.0f, 0.0f };

            Entity* m_LastCollisionEntity = nullptr;
    };
}
