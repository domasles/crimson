#pragma once

#include <Engine.h>

using namespace engine;

namespace crimson {
    class Player : public Entity {
        public:
            void init() override;
            void update(float deltaTime) override;
            void render() override;

        private:
            float m_Speed = 200;

            std::unique_ptr<InputSystem> m_InputSystem;
            std::unique_ptr<Map> m_Map;
    };
}
