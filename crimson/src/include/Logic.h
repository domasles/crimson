#pragma once

#include <entities/Player.h>
#include <utils/image.h>

#include <Scene.h>
#include <Input.h>

using namespace crimson::utils::image;
using namespace crimson::utils::math;

namespace crimson {
    class TestScene : public Scene {
        public:
            TestScene() { m_Name = "TestScene"; }

            void init() override;
            void update(const float deltaTime) override;
            void render() override;

        private:
            int m_ChangeCount = 0;
            float m_Speed = 200;

            Vector2 m_Position{ 300, 300 };
            
            std::unique_ptr<Player> m_Player;
    };

    class TestScene2 : public Scene {
        public:
            TestScene2() { m_Name = "TestScene2"; }

            void init() override {}
            void update(const float deltaTime) override;
            void render() override {}
    };

    class Logic {
        public:
            static Logic& getInstance() {
                static std::once_flag flag;
                static Logic* instance = nullptr;

                std::call_once(flag, []() { instance = new Logic(); });

                return *instance;
            }

            void init();
            void update();
            void render();

            InputSystem getInputSystem() { return m_InputSystem; }

        private:
            Logic() = default;

            Logic(const Logic&) = delete;
            Logic& operator=(const Logic&) = delete;

            InputSystem m_InputSystem;

            std::shared_ptr<TestScene> m_TestScene;
            std::shared_ptr<TestScene2> m_TestScene2;
    };
}
