#pragma once

#include <SDL3/SDL.h>

#include <Scene.h>
#include <Input.h>
#include <memory>

namespace crimson {
    class TestScene;
    class TestScene2;

    class TestScene : public Scene {
        public:
            TestScene() { m_Name = "TestScene"; }

            void init() override { m_TestScene2 = std::make_shared<TestScene2>(); }
            void update(float deltaTime) override;
            void render() override {}
            void cleanup() override {}

        private:
            int changeCount = 0;
            std::shared_ptr<TestScene2> m_TestScene2;
    };

    class TestScene2 : public Scene {
        public:
            TestScene2() { m_Name = "TestScene2"; }

            void init() override { m_TestScene = std::make_shared<TestScene>(); }
            void update(float deltaTime) override;
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
            void update() { SceneManager::getInstance().update(); }

            InputSystem getInputSystem() { return m_InputSystem; }

        private:
            InputSystem m_InputSystem;

            std::shared_ptr<TestScene> m_TestScene;
            std::shared_ptr<TestScene2> m_TestScene2;
    };
}
