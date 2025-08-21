#pragma once

#include <utils/math.h>
#include <collisions/types/BlockCollision.h>
#include <collisions/shapes/BoxShape.h>
#include <string>

using namespace engine::utils::math;
using namespace engine::collisions::types;
using namespace engine::collisions::shapes;

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

    struct CollisionTile {
        CollisionType type = CollisionType::BLOCK;
        CollisionShape shape = CollisionShape::BOX;
        Vector2 worldPosition;
        Vector2 size;
        int collisionValue;          // LDTk IntGrid value
        std::string layerIdentifier; // LDTk layer name
    };

    // Unified collision checking functions with automatic shape dispatch
    bool checkCollision(const Collision& entityCollision, const Vector2& entityPos,
                       const CollisionTile& tileCollision);
    
    bool checkCollision(const Collision& collision1, const Vector2& pos1,
                       const Collision& collision2, const Vector2& pos2);

    // Shape dispatch system for future-proofing with automatic registration
    namespace dispatch {
        // Function pointer type for collision checking
        using CollisionFunction = bool(*)(const Vector2&, const Vector2&, const Vector2&, const Vector2&);
        
        // Automatic shape collision dispatch
        bool checkShapeCollision(CollisionShape shape1, const Vector2& pos1, const Vector2& size1,
                                CollisionShape shape2, const Vector2& pos2, const Vector2& size2);
        
        // Shape registration system for automatic dispatch
        void registerShapeCollision(CollisionShape shape1, CollisionShape shape2, CollisionFunction func);
        CollisionFunction getCollisionFunction(CollisionShape shape1, CollisionShape shape2);
    }
}
