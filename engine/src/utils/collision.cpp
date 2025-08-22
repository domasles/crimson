#include <pch.h>

#include <utils/collision.h>

#include <collisions/shapes/BoxShape.h>
#include <collisions/types/BlockCollision.h>

using namespace engine::collisions::shapes;
using namespace engine::collisions::types;

namespace engine::utils::collision {
    Collision::Collision() {
        type = std::make_unique<BlockCollision>();
        shape = std::make_unique<BoxShape>();
    }

    Collision::Collision(const Vector2& size) : size(size) {
        type = std::make_unique<BlockCollision>();
        shape = std::make_unique<BoxShape>();
    }

    Collision::Collision(const Vector2& offset, const Vector2& size) : offset(offset), size(size) {
        type = std::make_unique<BlockCollision>();
        shape = std::make_unique<BoxShape>();
    }

    Collision::Collision(const Collision& other) : offset(other.offset), size(other.size) {
        if (other.type) type = other.type->clone();
        if (other.shape) shape = other.shape->clone();
    }

    Collision& Collision::operator=(const Collision& other) {
        if (this != &other) {
            offset = other.offset;
            size = other.size;

            if (other.type) type = other.type->clone();
            if (other.shape) shape = other.shape->clone();
        }

        return *this;
    }

    CollisionTile::CollisionTile() {
        type = std::make_unique<BlockCollision>();
        shape = std::make_unique<BoxShape>();
    }

    CollisionTile::CollisionTile(const CollisionTile& other) : worldPosition(other.worldPosition), size(other.size), collisionValue(other.collisionValue), layerIdentifier(other.layerIdentifier) {
        if (other.type) type = other.type->clone();
        if (other.shape) shape = other.shape->clone();
    }

    CollisionTile& CollisionTile::operator=(const CollisionTile& other) {
        if (this != &other) {
            worldPosition = other.worldPosition;
            size = other.size;
            collisionValue = other.collisionValue;
            layerIdentifier = other.layerIdentifier;

            if (other.type) type = other.type->clone();
            if (other.shape) shape = other.shape->clone();
        }

        return *this;
    }
}
