#pragma once

namespace engine::collisions::types {
    struct BlockCollision {
        static constexpr bool blocksMovement = true;
        static constexpr bool triggersEvents = false;
        
        static bool shouldBlockMovement() {
            return blocksMovement;
        }
        
        static bool shouldTriggerEvents() {
            return triggersEvents;
        }
    };
}
