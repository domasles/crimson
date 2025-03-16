#include <pch.h>

#include <entities/Player.h>
#include <utils/image.h>

#include <Core.h>

using namespace engine::utils::image;

namespace crimson {
    void Player::init() {
        m_EngineCore.getInputSystem()->loadInputActions("games/crimson/config/InputActions.json");

        m_Texture = loadImage(m_EngineCore.getRenderer(), "games/crimson/assets/Square.png");
        m_Stretch = { 100, 100 };
    }

    void Player::update(float deltaTime) {
        Vector2 movement = m_EngineCore.getInputSystem()->getMovementVector();
        addPosition(movement * m_Speed * deltaTime);
    }

    void Player::render() {
        if (!renderTexture(m_EngineCore.getRenderer(), m_Texture.get(), m_Position, m_Stretch)) return;
    }
}
