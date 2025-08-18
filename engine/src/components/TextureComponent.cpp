#include <pch.h>

#include <components/TransformComponent.h>
#include <components/TextureComponent.h>

#include <Entity.h>

namespace engine {
    void TextureComponent::render() {
        if (!m_Texture || !m_Entity) return;

        auto* transformComp = m_Entity->getComponent<TransformComponent>();
        if (!transformComp) return;

        m_Texture->render(transformComp->getSize(), transformComp->getPosition());
    }

    void TextureComponent::render(const SDL_FRect& cropRegion) {
        if (!m_Texture || !m_Entity) return;

        auto* transformComp = m_Entity->getComponent<TransformComponent>();
        if (!transformComp) return;

        Vector2 cropSize{cropRegion.w, cropRegion.h};
        Vector2 cropPosition{cropRegion.x, cropRegion.y};

        m_Texture->render(transformComp->getSize(), transformComp->getPosition(), cropSize, cropPosition);
    }

    void TextureComponent::render(const SDL_FRect& cropRegion, const SDL_FRect& destinationRegion) {
        if (!m_Texture) return;

        Vector2 destSize{destinationRegion.w, destinationRegion.h};
        Vector2 destPosition{destinationRegion.x, destinationRegion.y};
        Vector2 cropSize{cropRegion.w, cropRegion.h};
        Vector2 cropPosition{cropRegion.x, cropRegion.y};

        m_Texture->render(destSize, destPosition, cropSize, cropPosition);
    }
}
