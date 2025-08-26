#include <mapCollisionTiles/BaseTile.h>

namespace engine::mapCollisionTiles {
    void BaseTile::init() {
        init(Vector2{0.0f, 0.0f});
    }

    void BaseTile::init(const Vector2& position) {
        auto* transform = addComponent<TransformComponent>();
        auto* collision = addComponent<CollisionComponent>();

        transform->setPosition(position);
        
        configureCollision();
    }

    void BaseTile::update(float deltaTime) {
        updateComponents(deltaTime);
    }

    void BaseTile::setCollisionType(std::unique_ptr<CollisionType> newType) {
        auto* collision = getComponent<CollisionComponent>();

        if (collision) {
            collision->setCollisionType(std::move(newType));
        }
    }

    void BaseTile::configureCollision() {
        auto* collision = getComponent<CollisionComponent>();

        if (collision) {
            collision->setCollisionType(std::make_unique<BlockCollision>());
            collision->setCollisionShape(std::make_unique<BoxShape>());
            collision->setParticipatesInQueries(true);
        }
    }
}
