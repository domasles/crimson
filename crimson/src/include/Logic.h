#pragma once

#include <SDL3/SDL.h>

#include <Image.h>
#include <Scene.h>
#include <Input.h>
#include <Game.h>

#include <memory>

namespace crimson {
    class TestScene;
    class TestScene2;

    class TestScene : public Scene {
        public:
            TestScene() : m_TestTexture(nullptr, SDL_DestroyTexture) { m_Name = "TestScene"; }

            void init() override;
            void update(const float deltaTime) override;
            void render() override;
            void cleanup() override {}

        private:
            int changeCount = 0;

            float m_PlayerX = 300;
            float m_PlayerY = 300;

            float m_Speed = 200;

            std::shared_ptr<TestScene2> m_TestScene2;
            std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> m_TestTexture;
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
