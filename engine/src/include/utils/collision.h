#pragma once

#include <utils/math.h>

using namespace engine::utils::math;

namespace engine::utils::collision {
    enum class CollisionType {
        NONE = 0,   // No collision
        BLOCK = 1   // Solid collision that blocks movement
    };

    enum class CollisionShape {
        BOX = 0     // Rectangular collision box
    };

    struct Collision {
        CollisionType type = CollisionType::BLOCK;
        CollisionShape shape = CollisionShape::BOX;
        Vector2 offset{0, 0};   // Offset from entity position
        Vector2 size{0, 0};     // Size of collision box
        
        Collision() = default;
        Collision(const Vector2& size) : size(size) {}
        Collision(const Vector2& offset, const Vector2& size) : offset(offset), size(size) {}
        Collision(CollisionType type, CollisionShape shape, const Vector2& size) 
            : type(type), shape(shape), size(size) {}
        Collision(CollisionType type, CollisionShape shape, const Vector2& offset, const Vector2& size) 
            : type(type), shape(shape), offset(offset), size(size) {}
    };
}
