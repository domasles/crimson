#include <pch.h>

#include <entities/Player.h>

namespace crimson {
    void Player::init() {
        auto* transform = addComponent<TransformComponent>();
        auto* texture = addComponent<TextureComponent>();
        auto* input = addComponent<InputComponent>();
        auto* collision = addComponent<CollisionComponent>();

        texture->setTexture(loadTexture("assets/tilesets", "TX Tileset Ground.png"));
        transform->setSize({100, 100});
    }

    void Player::update(float deltaTime) {
        updateComponents(deltaTime);

        auto* input = getComponent<InputComponent>();
        auto* transform = getComponent<TransformComponent>();

        if (input && transform && input->getInputSystem()) {
            Vector2 movement = input->getMovementVector();
            transform->addPosition(movement * m_Speed * deltaTime);
        }
    }

    void Player::render() {
        auto* texture = getComponent<TextureComponent>();
        auto* transform = getComponent<TransformComponent>();

        if (texture && texture->hasTexture() && transform) {
            Vector2 cRegion{0, 0};
            Vector2 cSize{16, 16};

            auto texturePtr = texture->getTexture();
            texturePtr->render(transform->getSize(), transform->getPosition(), cSize, cRegion);
        }
    }
}
