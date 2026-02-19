#pragma once

#include <utils/collision.h>
#include <collisions/CollisionShape.h>

using namespace engine::utils::math;
using namespace engine::utils::collision;
using namespace engine::collisions;

namespace engine::utils::gjk {
    CollisionResult test(
        const CollisionShape& shapeA, const Vector2& posA, const Vector2& sizeA,
        const CollisionShape& shapeB, const Vector2& posB, const Vector2& sizeB
    );
}
