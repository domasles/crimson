#include <pch.h>

#include <components/BoxRendererComponent.h>
#include <components/TransformComponent.h>

#include <Entity.h>
#include <Core.h>

namespace engine {
    void BoxRendererComponent::render() {
        auto* transform = getEntity()->getComponent<TransformComponent>();
        if (!transform) return;

        GLRenderer* renderer = getCore().getGLRenderer();
        if (!renderer) return;

        renderer->drawQuad(
            transform->getPosition(),
            transform->getSize(),
            0, // No texture
            m_Color
        );
    }
}
