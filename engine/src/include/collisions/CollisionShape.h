#pragma once

#include <utils/math.h>

using namespace engine::utils::math;
using namespace engine::utils::collision;

namespace engine::collisions {
    class CollisionShape {
        public:
            virtual ~CollisionShape() = default;

            virtual bool checkCollision(const Vector2& myPos, const Vector2& mySize, const CollisionShape& other, const Vector2& otherPos, const Vector2& otherSize) const = 0;
            virtual bool checkCollisionWithBox(const Vector2& myPos, const Vector2& mySize, const Vector2& boxPos, const Vector2& boxSize) const = 0;

            virtual std::unique_ptr<CollisionShape> clone() const = 0;
        };
}
