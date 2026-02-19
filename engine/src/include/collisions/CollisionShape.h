#pragma once

#include <utils/math.h>
#include <utils/rendering.h>

#include <GLRenderer.h>

using namespace engine::utils::math;
using namespace engine::utils::rendering;

namespace engine::collisions {
    class CollisionShape {
        public:
            virtual ~CollisionShape() = default;

            virtual Vector2 getSupportPoint(const Vector2& position, const Vector2& size, const Vector2& direction) const = 0;
            virtual AABB getBoundingBox(const Vector2& position, const Vector2& size) const = 0;

            virtual void renderGizmo(GLRenderer* renderer, const Vector2& position, const Vector2& size, const Color& color, float rotation = 0.0f) const = 0;

            virtual std::unique_ptr<CollisionShape> clone() const = 0;
    };
}
