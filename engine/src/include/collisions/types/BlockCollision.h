#pragma once

#include <collisions/CollisionType.h>

namespace engine::collisions::types {
    class BlockCollision : public CollisionType {
    public:
        bool shouldBlock() const override { return true; }
        bool shouldTriggerEvents() const override { return false; }
        
        std::unique_ptr<CollisionType> clone() const override { return std::make_unique<BlockCollision>(*this); }
    };
}
