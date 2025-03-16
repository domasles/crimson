#include <pch.h>

#include <levels/Test.h>

namespace crimson {
    void TestScene::init() {
        if (!m_Player) {
            m_Player = std::make_unique<crimson::Player>();

            m_Player->init();
            m_Player->setPosition({ 300, 300 });
        }
    }

    void TestScene::update(const float deltaTime) {
        m_Player->update(deltaTime);
    }

    void TestScene::render() {
        m_Player->render();
    }
}
