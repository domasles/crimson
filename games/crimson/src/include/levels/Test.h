#pragma once

#include <entities/Player.h>

#include <Scene.h>
#include <Input.h>

using namespace engine;

namespace crimson {
    class TestScene : public Scene {
        public:
            TestScene() { m_Name = "TestScene"; }

            void init() override;
            void update(const float deltaTime) override;
            void render() override;

        private:
            std::unique_ptr<crimson::Player> m_Player;
    };
}
