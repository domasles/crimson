#include <pch.h>

#include <entities/Player.h>

namespace crimson {
    void Player::init() {
        setTexture(loadTexture("assets/tilesets", "TX Tileset Ground.png"));
        getTransform().setSize({100, 100});
    }

    void Player::update(float deltaTime) {
        if (m_InputSystem) {
            Vector2 movement = m_InputSystem->getMovementVector();
            addPosition(movement * m_Speed * deltaTime);
        }
    }

    void Player::render() {
        if (hasTexture()) {
            Vector2 cRegion{0, 0};
            Vector2 cSize{16, 16};
            
            auto texture = getTexture();
            const auto& transform = getTransform();

            texture->render(transform.getSize(), transform.getPosition(), cSize, cRegion);
        }
    }
}
