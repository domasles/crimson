#pragma once

#include <collisions/CollisionType.h>

namespace engine::collisions::types {
    class NoneCollision : public CollisionType {
    public:
        bool shouldBlock() const override { return false; }
        bool shouldTriggerEvents() const override { return false; }

        std::unique_ptr<CollisionType> clone() const override { return std::make_unique<NoneCollision>(*this); }
    };
}
