#pragma once

#include <Engine.h>

using namespace engine;

namespace crimson {
    class Player : public Entity {
        public:
            void init() override;
            void update(float deltaTime) override;
            void render() override;

            void setInputSystem(InputSystem* inputSystem) { m_InputSystem = inputSystem; }

        private:
            float m_Speed = 200;
            InputSystem* m_InputSystem = nullptr;
    };
}
