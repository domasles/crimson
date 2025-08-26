#include <pch.h>

#include <entities/Player2.h>

using namespace engine::collisions::types;
using namespace engine::collisions::shapes;
using namespace engine::utils::collision;

namespace crimson {
    void Player2::init() {
        auto* transform = addComponent<TransformComponent>();
        auto* texture = addComponent<TextureComponent>();
        auto* collision = addComponent<CollisionComponent>();

        texture->setTexture(loadTexture("assets/tilesets", "TX Tileset Ground.png"));
        transform->setSize({100, 100});

        collision->setCollisionType(std::make_unique<BlockCollision>());
        collision->setCollisionShape(std::make_unique<BoxShape>());
        collision->setParticipatesInQueries(true);

        if (getSceneManager().getCurrentScene()->hasMap()) {
            auto* transformComp = getComponent<TransformComponent>();

            if (transformComp) {
                transformComp->setPosition(getSceneManager().getCurrentScene()->getMap()->getEntityPosition("Enemy"));
            }
        }
    }

    void Player2::update(float deltaTime) {
        updateComponents(deltaTime);

        auto* input = getComponent<InputComponent>();
        auto* transform = getComponent<TransformComponent>();
        auto* collision = getComponent<CollisionComponent>();

        if (input && transform && collision && input->getInputSystem()) {
            Vector2 movement = input->getMovementVector();
            Vector2 currentPos = transform->getPosition();
            Vector2 newPosition = currentPos + (movement * m_Speed * deltaTime);

            MultiCollisionResult collisions = collision->getAllCollisionsAt(newPosition);
            
            if (!collisions.hasBlockingCollision()) {
                transform->setPosition(newPosition);
            }
        }
    }

    void Player2::render() {
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
