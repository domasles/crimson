#include <pch.h>

#include <scenes/StartScene.h>

using namespace engine;
using namespace engine::ui;

namespace outBreak {
    void StartScene::init() {
        setBackgroundColor(Color(0.05f, 0.05f, 0.1f, 1.0f));
        setFont("assets/fonts/inter.ttf", true);

        m_Document = loadDocument("assets/ui/start.rml");
        m_Document.listen("play-btn", "click", []() { switchToScene("GameScene"); });
    }
}

