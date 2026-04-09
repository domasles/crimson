#pragma once

namespace engine::collisions {
    enum class CollisionType {
        Block,
        Trigger,
        None
    };

    inline bool shouldBlock(CollisionType type) {
        return type == CollisionType::Block;
    }

    inline bool shouldTriggerEvents(CollisionType type) {
        return type == CollisionType::Trigger;
    }
}
