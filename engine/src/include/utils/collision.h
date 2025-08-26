#pragma once

#include <utils/math.h>

#include <collisions/CollisionShape.h>
#include <collisions/CollisionType.h>

using namespace engine::utils::math;
using namespace engine::collisions;

namespace engine {
    class Entity;
}

namespace engine::utils::collision {
    struct CollisionResult {
        bool hasCollision = false;
        const CollisionType* hitType = nullptr;

        Entity* hitEntity = nullptr;

        Vector2 contactPoint{0, 0};
        Vector2 contactNormal{0, 0};

        bool isBlocking() const;
        bool isTrigger() const;
    };

    struct MultiCollisionResult {
        std::vector<CollisionResult> collisions;
        
        bool hasAnyCollision() const { return !collisions.empty(); }
        bool hasBlockingCollision() const;

        CollisionResult getFirstBlocking() const;

        std::vector<CollisionResult> getAllBlocking() const;
        std::vector<CollisionResult> getAllTriggers() const;
    };

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
}
