#pragma once

#include <entities/Player.h>
#include <utils/image.h>

#include <Scene.h>
#include <Input.h>

using namespace crimson::utils::image;
using namespace crimson::utils::math;

namespace crimson {
    class TestScene;
    class TestScene2;

    class TestScene : public Scene {
        public:
            TestScene() { m_Name = "TestScene"; }

            void init() override;
            void update(const float deltaTime) override;
            void render() override;
            void cleanup() override {}

        private:
            int m_ChangeCount = 0;
            float m_Speed = 200;

            Vector2 m_Position = { 300, 300 };
            
            std::shared_ptr<Player> m_Player;
            std::shared_ptr<TestScene2> m_TestScene2;
    };

    class TestScene2 : public Scene {
        public:
            TestScene2() { m_Name = "TestScene2"; }

            void init() override;
            void update(const float deltaTime) override;
            void render() override {}
            void cleanup() override {}

        private:
            std::shared_ptr<TestScene> m_TestScene;
    };

    class Logic {
        public:
            static Logic& getInstance() {
                static Logic instance;
                return instance;
            }

            void init();
            void update();
            void render();

            InputSystem getInputSystem() { return m_InputSystem; }

        private:
            Logic() {}

            Logic(const Logic&) = delete;
            Logic& operator=(const Logic&) = delete;

            InputSystem m_InputSystem;

            std::shared_ptr<TestScene> m_TestScene;
            std::shared_ptr<TestScene2> m_TestScene2;
    };
}
