#include <components/TransformComponent.h>

#include <Entity.h>

namespace engine {
    void TransformComponent::setPosition(const Vector2& position) {
        m_PreviousPosition = position;
        m_Transform.setPosition(position);
        m_InterpolatedPosition = position;
    }

    void TransformComponent::move(const Vector2& movement) {
        m_PreviousPosition = m_Transform.getPosition();
        Vector2 newPosition = m_Transform.getPosition() + movement;
        m_Transform.setPosition(newPosition);
        m_InterpolatedPosition = newPosition;
    }

    void TransformComponent::setRotation(float rotation) {
        m_PreviousRotation = rotation;
        m_Transform.setRotation(rotation);
        m_InterpolatedRotation = rotation;
    }

    void TransformComponent::rotate(float delta) {
        m_PreviousRotation = m_Transform.getRotation();
        float newRotation = m_Transform.getRotation() + delta;
        m_Transform.setRotation(newRotation);
        m_InterpolatedRotation = newRotation;
    }

    void TransformComponent::setSize(const Vector2& size) {
        m_PreviousSize = size;
        m_Transform.setSize(size);
        m_InterpolatedSize = size;
    }

    void TransformComponent::addSize(const Vector2& delta) {
        m_PreviousSize = m_Transform.getSize();
        Vector2 newSize = m_Transform.getSize() + delta;
        m_Transform.setSize(newSize);
        m_InterpolatedSize = newSize;
    }

    void TransformComponent::setScale(const Vector2& scale) {
        m_PreviousScale = scale;
        m_Transform.setScale(scale);
        m_InterpolatedScale = scale;
    }

    void TransformComponent::addScale(const Vector2& delta) {
        m_PreviousScale = m_Transform.getScale();
        Vector2 newScale = m_Transform.getScale() + delta;
        m_Transform.setScale(newScale);
        m_InterpolatedScale = newScale;
    }
}
