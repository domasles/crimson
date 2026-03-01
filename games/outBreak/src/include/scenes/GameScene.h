#pragma once

#include <Engine.h>

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

            Ball* m_Ball;
            Paddle* m_Paddle;
            GameManager* m_GameManager;

            std::vector<Brick*> m_Bricks;
    };
}
