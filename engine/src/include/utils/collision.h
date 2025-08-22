#pragma once

#include <utils/math.h>
#include <collisions/CollisionShape.h>
#include <collisions/CollisionType.h>
#include <string>
#include <memory>

using namespace engine::utils::math;
using namespace engine::collisions;

namespace engine::utils::collision {
    struct Collision {
        std::unique_ptr<CollisionType> type;
        std::unique_ptr<CollisionShape> shape;

        Vector2 offset{0, 0};
        Vector2 size{0, 0};

        Collision();
        Collision(const Vector2& size);
        Collision(const Vector2& offset, const Vector2& size);
        Collision(const Collision& other);

        Collision& operator=(const Collision& other);
        Collision(Collision&&) = default;
        Collision& operator=(Collision&&) = default;
    };

    struct CollisionTile {
        std::unique_ptr<CollisionType> type;
        std::unique_ptr<CollisionShape> shape;
        std::string layerIdentifier;

        Vector2 worldPosition;
        Vector2 size;

        int collisionValue;
        
        CollisionTile();
        CollisionTile(const CollisionTile& other);

        CollisionTile& operator=(const CollisionTile& other);
        CollisionTile(CollisionTile&&) = default;
        CollisionTile& operator=(CollisionTile&&) = default;
    };
}
