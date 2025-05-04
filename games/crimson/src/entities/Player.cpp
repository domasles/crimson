#include <pch.h>

#include <entities/Player.h>

#include <Core.h>

namespace crimson {
    void Player::init() {
        m_InputSystem = std::make_unique<InputSystem>("config");
        m_InputSystem->loadInputActions("InputActions.json");

        m_Texture = std::make_shared<Texture>("assets/tilesets");
        m_Texture->loadImage("TX Tileset Ground.png");

        setPosition({ 300, 300 });
    }

    void Player::update(float deltaTime) {
        Vector2 movement = m_InputSystem->getMovementVector();
        addPosition(movement * m_Speed * deltaTime);
    }

    void Player::render() {
        Vector2 cRegion{ 0, 0 };
        Vector2 cSize{ 16, 16 };

        if (!m_Texture->render(m_Size, m_Position, cSize, cRegion)) return;
    }
}
