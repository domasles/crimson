#pragma once

#include <collisions/CollisionType.h>

namespace engine::collisions::types {
    class TriggerCollision : public CollisionType {
        public:
            bool shouldBlock() const override { return false; }
            bool shouldTriggerEvents() const override { return true; }
            
            std::unique_ptr<CollisionType> clone() const override { return std::make_unique<TriggerCollision>(*this); }
    };
}
