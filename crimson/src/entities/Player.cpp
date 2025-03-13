#include <pch.h>

#include <entities/Player.h>
#include <utils/image.h>

#include <Game.h>

namespace crimson {
    void Player::init() {
        m_Texture = loadImage(Game::getInstance().getRenderer(), "assets/Square.png");
        m_Stretch = { 100, 100 };
    }

    void Player::render() {
        if (!renderTexture(Game::getInstance().getRenderer(), m_Texture.get(), m_Position, m_Stretch)) {
            return;
        }
    }
}
