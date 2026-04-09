#include <mapCollisionTiles/MapTile.h>
#include <collisions/shapes/BoxShape.h>

using namespace engine::collisions::shapes;

namespace engine::mapCollisionTiles {
    void MapTile::init() {
        init(Vector2{0.0f, 0.0f});
    }

    void MapTile::init(const Vector2& position) {
        init(position, m_TileSize);
    }

    void MapTile::init(const Vector2& position, const Vector2& size) {
        m_TileSize = size;
        
        auto* transform = addComponent<TransformComponent>();
        auto* collision = addComponent<CollisionComponent>();

        transform->setPosition(position);
        transform->setSize(size);
        
        configureCollision();
    }

    void MapTile::setTileSize(const Vector2& size) {
        m_TileSize = size;
        auto* transform = getComponent<TransformComponent>();

        if (transform) {
            transform->setSize(size);
        }
    }

    void MapTile::configureCollision() {
        auto* collision = getComponent<CollisionComponent>();

        if (collision) {
            collision->setCollisionType(CollisionType::Block);
            collision->setCollisionShape(std::make_unique<BoxShape>());
            collision->setParticipatesInQueries(true);
        }
    }
}
