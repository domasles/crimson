#include <pch.h>

#include <scenes/Level1.h>

namespace crimson {
    void Level1::init() {
        if (!m_Player) {
            m_Player = std::make_unique<Player>();

            m_Player->init();
            m_Player->setPosition({ 300, 300 });
        }
    }

    void Level1::update(const float deltaTime) {
        m_Player->update(deltaTime);
    }

    void Level1::render() {
        m_Player->render();
    }
}
