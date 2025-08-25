#include <components/TransformComponent.h>

#include <Entity.h>

namespace engine {
    void TransformComponent::setPosition(const Vector2& position) {
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
