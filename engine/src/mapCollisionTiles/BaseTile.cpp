#include <mapCollisionTiles/BaseTile.h>
#include <collisions/shapes/BoxShape.h>

using namespace engine::collisions::shapes;

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

    void BaseTile::setCollisionType(CollisionType newType) {
        auto* collision = getComponent<CollisionComponent>();

        if (collision) {
            collision->setCollisionType(newType);
        }
    }

    void BaseTile::configureCollision() {
        auto* collision = getComponent<CollisionComponent>();

        if (collision) {
            collision->setCollisionType(CollisionType::Block);
            collision->setCollisionShape(std::make_unique<BoxShape>());
            collision->setParticipatesInQueries(true);
        }
    }
}
