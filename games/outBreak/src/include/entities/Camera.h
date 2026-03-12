#pragma once

#include <Engine.h>

using namespace engine;

namespace outBreak {
    class Camera : public Entity {
        public:
            void init() override;
            void update(float deltaTime) override {}
            void render() override {}
    };
}
