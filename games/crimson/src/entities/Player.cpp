#include <pch.h>

#include <entities/Player.h>

#include <Core.h>

namespace crimson {
    void Player::init() {
        m_EngineCore.getInputSystem()->loadInputActions("config/InputActions.json");
        m_Texture = std::make_shared<Texture>("assets/Square.png");
    }

    void Player::update(float deltaTime) {
        Vector2 movement = m_EngineCore.getInputSystem()->getMovementVector();
        addPosition(movement * m_Speed * deltaTime);
    }

    void Player::render() {
        if (!m_Texture->render(m_Size, m_Position)) return;
    }
}
