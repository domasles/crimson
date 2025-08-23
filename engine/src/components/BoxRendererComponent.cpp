#include <pch.h>

#include <components/BoxRendererComponent.h>
#include <components/TransformComponent.h>
#include <Entity.h>
#include <Core.h>

namespace engine {
    void BoxRendererComponent::render() {
        auto* transform = getEntity()->getComponent<TransformComponent>();
        if (!transform) {
            return; // Cannot render without transform
        }

        SDL_Renderer* renderer = getCore().getRenderer();
        if (!renderer) {
            return;
        }

        // Set the draw color
        SDL_SetRenderDrawColor(renderer, 
            static_cast<Uint8>(m_Color.r * 255), 
            static_cast<Uint8>(m_Color.g * 255), 
            static_cast<Uint8>(m_Color.b * 255), 
            static_cast<Uint8>(m_Color.a * 255));

        // Create rectangle from transform
        SDL_FRect rect;
        rect.x = transform->getPosition().getX();
        rect.y = transform->getPosition().getY();
        rect.w = transform->getSize().getX();
        rect.h = transform->getSize().getY();

        // Render filled rectangle
        SDL_RenderFillRect(renderer, &rect);
    }
}
