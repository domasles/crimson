#pragma once

namespace engine::collisions {
    class CollisionType {
    public:
        virtual ~CollisionType() = default;
        
        virtual bool shouldBlock() const = 0;
        virtual bool shouldTriggerEvents() const = 0;
        
        virtual std::unique_ptr<CollisionType> clone() const = 0;
    };
}
