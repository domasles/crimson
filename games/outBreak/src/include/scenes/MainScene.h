#pragma once

#include <Engine.h>
#include <vector>

using namespace engine;

namespace outBreak {
    class Ball;
    class Paddle;
    class Brick;
    class GameManager;

    class MainScene : public Scene {
        public:
            MainScene() { m_Name = "MainScene"; }

            void init() override;
            void update(const float deltaTime) override;
            void render() override;

        private:
            Ball* m_Ball;
            Paddle* m_Paddle;
            GameManager* m_GameManager;
            std::vector<Brick*> m_Bricks;

            void createBricks();
    };
}
