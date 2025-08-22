#include <pch.h>

#include <components/InputComponent.h>

namespace engine {
    Vector2 InputComponent::getMovementVector() const {
        if (!m_InputSystem) return Vector2(0, 0);
        return m_InputSystem->getMovementVector();
    }

    bool InputComponent::isActionPressed(const std::string& action) const {
        if (!m_InputSystem) return false;
        return m_InputSystem->isActionPressed(action);
    }
}
