#include <pch.h>

#include <entities/Player.h>

#include <components/TransformComponent.h>
#include <components/TextureComponent.h>
#include <components/InputComponent.h>

namespace crimson {
    void Player::init() {
        auto* transform = addComponent<engine::TransformComponent>();
        auto* texture = addComponent<engine::TextureComponent>();
        auto* input = addComponent<engine::InputComponent>();

        texture->setTexture(loadTexture("assets/tilesets", "TX Tileset Ground.png"));
        transform->setSize({100, 100});
    }

    void Player::update(float deltaTime) {
        updateComponents(deltaTime);

        auto* input = getComponent<engine::InputComponent>();
        auto* transform = getComponent<engine::TransformComponent>();

        if (input && transform && input->getInputSystem()) {
            Vector2 movement = input->getMovementVector();
            transform->addPosition(movement * m_Speed * deltaTime);
        }
    }

    void Player::render() {
        auto* texture = getComponent<engine::TextureComponent>();
        auto* transform = getComponent<engine::TransformComponent>();

        if (texture && texture->hasTexture() && transform) {
            Vector2 cRegion{0, 0};
            Vector2 cSize{16, 16};

            auto texturePtr = texture->getTexture();
            texturePtr->render(transform->getSize(), transform->getPosition(), cSize, cRegion);
        }
    }
}
