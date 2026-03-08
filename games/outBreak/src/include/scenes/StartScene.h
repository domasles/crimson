#pragma once

#include <Engine.h>

using namespace engine;

namespace outBreak {
    class StartScene : public Scene {
        public:
            StartScene() { m_Name = "StartScene"; }

            void init() override;
            void update(float deltaTime) override {}
            void render() override {}

        private:
            UIDocument m_Document;
    };
}

