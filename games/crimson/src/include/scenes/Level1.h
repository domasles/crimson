#pragma once

#include <entities/Player.h>
#include <entities/Player2.h>

#include <Engine.h>

using namespace engine;

namespace crimson {
    class Level1 : public Scene {
        public:
            Level1() { m_Name = "Level1"; }

            void init() override;
            void update(const float deltaTime) override;
            void render() override;

        private:
            Player* m_Player = nullptr;
            Player2* m_Player2 = nullptr;
    };
}
