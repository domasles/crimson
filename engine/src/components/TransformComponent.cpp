#include <components/TransformComponent.h>

#include <Entity.h>
#include <cmath>

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

    void TransformComponent::setParent(Entity* parent) {
        if (parent == m_Entity) return;

        if (m_Parent != nullptr) {
            TransformComponent* oldParentTransform = m_Parent->getComponent<TransformComponent>();

            if (oldParentTransform != nullptr) {
                auto& children = oldParentTransform->m_Children;
                children.erase(std::remove(children.begin(), children.end(), m_Entity), children.end());
            }
        }

        if (parent != nullptr) {
            TransformComponent* parentTransform = parent->getComponent<TransformComponent>();

            if (parentTransform != nullptr) {
                Entity* ancestor = parentTransform->getParent();
                while (ancestor != nullptr) {
                    if (ancestor == m_Entity) return;

                    TransformComponent* ancestorTransform = ancestor->getComponent<TransformComponent>();
                    ancestor = ancestorTransform ? ancestorTransform->getParent() : nullptr;
                }
            }
        }

        m_Parent = parent;

        if (m_Parent != nullptr) {
            TransformComponent* parentTransform = m_Parent->getComponent<TransformComponent>();

            if (parentTransform != nullptr) {
                parentTransform->m_Children.push_back(m_Entity);
            }
        }
    }

    void TransformComponent::detach() {
        if (m_Parent != nullptr) {
            TransformComponent* parentTransform = m_Parent->getComponent<TransformComponent>();

            if (parentTransform != nullptr) {
                auto& children = parentTransform->m_Children;
                children.erase(std::remove(children.begin(), children.end(), m_Entity), children.end());
            }
        }

        m_Parent = nullptr;
    }

    Vector2 TransformComponent::getWorldPosition() const {
        Vector2 worldPos = m_Transform.getPosition();
        
        if (m_Parent != nullptr) {
            TransformComponent* parentTransform = m_Parent->getComponent<TransformComponent>();

            if (parentTransform != nullptr) {
                Vector2 parentWorldPos = parentTransform->getWorldPosition();
                Vector2 parentScale = parentTransform->getWorldScale();

                float parentRotation = parentTransform->getWorldRotation();
                float rotRad = parentRotation * (3.14159265359f / 180.0f);
                float cosR = std::cos(rotRad);
                float sinR = std::sin(rotRad);

                Vector2 rotatedPos{
                    worldPos.getX() * cosR - worldPos.getY() * sinR,
                    worldPos.getX() * sinR + worldPos.getY() * cosR
                };

                worldPos = Vector2{
                    parentWorldPos.getX() + rotatedPos.getX() * parentScale.getX(),
                    parentWorldPos.getY() + rotatedPos.getY() * parentScale.getY()
                };
            }
        }

        return worldPos;
    }

    float TransformComponent::getWorldRotation() const {
        float worldRot = m_Transform.getRotation();

        if (m_Parent != nullptr) {
            TransformComponent* parentTransform = m_Parent->getComponent<TransformComponent>();

            if (parentTransform != nullptr) {
                worldRot += parentTransform->getWorldRotation();
            }
        }

        return worldRot;
    }

    Vector2 TransformComponent::getWorldScale() const {
        Vector2 worldScale = m_Transform.getScale();

        if (m_Parent != nullptr) {
            TransformComponent* parentTransform = m_Parent->getComponent<TransformComponent>();

            if (parentTransform != nullptr) {
                Vector2 parentScale = parentTransform->getWorldScale();

                Vector2 scaledVec{
                    worldScale.getX() * parentScale.getX(),
                    worldScale.getY() * parentScale.getY()
                };
                worldScale = scaledVec;
            }
        }

        return worldScale;
    }
}
