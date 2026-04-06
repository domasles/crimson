#include <pch.h>

#include <scenes/StartScene.h>

using namespace engine;
using namespace engine::ui;

namespace outBreak {
    void StartScene::init() {
        setFont("assets/fonts/inter/inter.ttf", true);

        m_Document.setDocument("assets/ui/start.rml");
        m_Document.listen("play-btn", "click", []() { switchToScene("GameScene"); });
    }
}

