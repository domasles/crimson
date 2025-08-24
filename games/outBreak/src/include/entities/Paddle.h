#pragma once

#include <Engine.h>

using namespace engine;

namespace outBreak {
    class Paddle : public Entity {
        public:
            Paddle() = default;
            virtual ~Paddle() = default;

            void init() override;
            void update(float deltaTime) override;
            void render() override;

        private:
            float m_Speed = 600.0f; // pixels per second
            static constexpr float PADDLE_WIDTH = 120.0f;
            static constexpr float PADDLE_HEIGHT = 20.0f;
    };
}
