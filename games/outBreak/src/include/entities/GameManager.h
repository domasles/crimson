#pragma once

#include <Engine.h>

using namespace engine;

namespace outBreak {
    class Ball;
    class Paddle;
    class Brick;

    class GameManager : public Entity {
        public:
            GameManager() = default;
            virtual ~GameManager() = default;

            void init() override {}
            void update(float deltaTime) override;
            void render() override {}

            void setBall(Ball* ball) { m_Ball = ball; }
            void setPaddle(Paddle* paddle) { m_Paddle = paddle; }
            void addBrick(Brick* brick) { m_Bricks.push_back(brick); }
            void resetGame();

        private:
            Ball* m_Ball = nullptr;
            Paddle* m_Paddle = nullptr;
            std::vector<Brick*> m_Bricks;

            void checkCollisions();
            void checkBallPaddleCollision();
            void checkBallBrickCollisions();
            void checkBallBounds();
            void triggerWaveEffect(Vector2 origin);
            bool allBricksDestroyed();
    };
}
