#pragma once

#include <Engine.h>

#include <entities/Camera.h>

using namespace engine;

namespace outBreak {
    class Ball;
    class Paddle;
    class Brick;
    class GameManager;

    class GameScene : public Scene {
        public:
            GameScene() { m_Name = "GameScene"; }

            void init() override;
            void update(const float deltaTime) override;
            void render() override;

        private:
            void createBricks();

            Ball* m_Ball = nullptr;
            Paddle* m_Paddle = nullptr;
            Camera* m_Camera = nullptr;
            GameManager* m_GameManager = nullptr;

            std::vector<Brick*> m_Bricks;
    };
}
