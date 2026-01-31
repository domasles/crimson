#pragma once

#include <utils/math.h>
#include <utils/rendering.h>

#include <GLRenderer.h>

namespace engine::utils::collision { struct CollisionResult; }

using namespace engine::utils::math;
using namespace engine::utils::collision;
using namespace engine::utils::rendering;

namespace engine::collisions {
    class CollisionShape {
        public:
            virtual ~CollisionShape() = default;

            virtual CollisionResult checkCollision(const Vector2& myPos, const Vector2& mySize, const CollisionShape& other, const Vector2& otherPos, const Vector2& otherSize) const = 0;
            virtual CollisionResult checkCollisionWithBox(const Vector2& myPos, const Vector2& mySize, const Vector2& boxPos, const Vector2& boxSize) const = 0;
            virtual CollisionResult checkCollisionWithCircle(const Vector2& myPos, const Vector2& mySize, const Vector2& circlePos, const Vector2& circleSize) const = 0;

            virtual void renderGizmo(GLRenderer* renderer, const Vector2& position, const Vector2& size, const Color& color) const = 0;

            virtual std::unique_ptr<CollisionShape> clone() const = 0;
    };
}
