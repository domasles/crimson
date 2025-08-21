#include <components/TransformComponent.h>
#include <components/CollisionComponent.h>
#include <Entity.h>

namespace engine {

    void TransformComponent::setPosition(const Vector2& position) {
        // Check if entity has collision component
        if (auto collision = m_Entity->getComponent<CollisionComponent>()) {
            if (collision->isEnabled()) {
                // Calculate movement from current position
                Vector2 movement = position - m_Transform.getPosition();
                
                // Use collision-aware movement
                if (collision->canMoveTo(position)) {
                    m_Transform.setPosition(position);
                }
                // If collision blocks movement, position stays the same
                return;
            }
        }
        
        // No collision component or collision disabled, move freely
        m_Transform.setPosition(position);
    }

    void TransformComponent::addPosition(const Vector2& offset) {
        Vector2 newPosition = m_Transform.getPosition() + offset;
        setPosition(newPosition);  // Use collision-aware setPosition
    }

    void TransformComponent::move(const Vector2& movement) {
        Vector2 newPosition = m_Transform.getPosition() + movement;
        setPosition(newPosition);  // Use collision-aware setPosition
    }

}
