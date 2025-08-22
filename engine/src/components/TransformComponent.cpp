#include <components/TransformComponent.h>
#include <components/CollisionComponent.h>

#include <Entity.h>

namespace engine {
    void TransformComponent::setPosition(const Vector2& position) {
        if (auto collision = m_Entity->getComponent<CollisionComponent>()) {
            if (collision->isEnabled()) {
                Vector2 movement = position - m_Transform.getPosition();

                if (collision->canMoveTo(position)) {
                    m_Transform.setPosition(position);
                }

                return;
            }
        }

        m_Transform.setPosition(position);
    }

    void TransformComponent::addPosition(const Vector2& offset) {
        Vector2 newPosition = m_Transform.getPosition() + offset;
        setPosition(newPosition);
    }

    void TransformComponent::move(const Vector2& movement) {
        Vector2 newPosition = m_Transform.getPosition() + movement;
        setPosition(newPosition);
    }

}
