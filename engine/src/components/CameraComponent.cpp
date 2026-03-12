#include <pch.h>

#include <components/CameraComponent.h>
#include <components/TransformComponent.h>
#include <Scene.h>

namespace engine {
    void CameraComponent::setPrimary() {
        if (auto* scene = getSceneManager().getCurrentSceneRaw()) {
            scene->setPrimaryCamera(this);
        }
    }

    Vector2 CameraComponent::screenToWorld(const Vector2& screenPos, float viewW, float viewH) const {
        auto* transform = m_Entity ? m_Entity->getComponent<TransformComponent>() : nullptr;
        if (!transform) return screenPos;

        Vector2 camPos = transform->getInterpolatedPosition();

        return Vector2{
            screenPos.getRawX() / m_Zoom + camPos.getRawX() - viewW / (2.0f * m_Zoom),
            screenPos.getRawY() / m_Zoom + camPos.getRawY() - viewH / (2.0f * m_Zoom)
        };
    }
}
