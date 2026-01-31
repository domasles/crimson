#include <pch.h>

#include <components/CollisionComponent.h>
#include <components/TransformComponent.h>
#include <components/TextureComponent.h>

#include <Entity.h>
#include <Gizmos.h>
#include <Scene.h>
#include <Core.h>

namespace engine {
    bool Gizmos::s_CollisionBoundsEnabled = false;
    bool Gizmos::s_TextureBoundsEnabled = false;

    void Gizmos::setChannel(GizmoType type, bool enabled) {
        switch (type) {
            case GizmoType::COLLISION_BOUNDS:
                s_CollisionBoundsEnabled = enabled;
                break;

            case GizmoType::TEXTURE_BOUNDS:
                s_TextureBoundsEnabled = enabled;
                break;
        }
    }

    bool Gizmos::isChannelEnabled(GizmoType type) {
        switch (type) {
            case GizmoType::COLLISION_BOUNDS:
                return s_CollisionBoundsEnabled;

            case GizmoType::TEXTURE_BOUNDS:
                return s_TextureBoundsEnabled;
        }

        return false;
    }

    void Gizmos::enableAll() {
        s_CollisionBoundsEnabled = true;
        s_TextureBoundsEnabled = true;
    }

    void Gizmos::disableAll() {
        s_CollisionBoundsEnabled = false;
        s_TextureBoundsEnabled = false;
    }

    void Gizmos::renderGizmos() {
        GLRenderer* renderer = getCore().getGLRenderer();
        if (!renderer) return;

        auto currentScene = getSceneManager().getCurrentScene();
        if (!currentScene) return;

        const auto& entities = currentScene->getEntities();

        for (const auto& entityPtr : entities) {
            Entity* entity = entityPtr.get();
            if (!entity) continue;

            auto* transform = entity->getComponent<TransformComponent>();
            if (!transform) continue;

            Vector2 position = transform->getPosition();
            Vector2 size = transform->getSize();

            #ifdef ENGINE_DEBUG
                // Draw collision bounds
                if (s_CollisionBoundsEnabled) {
                    auto* collision = entity->getComponent<CollisionComponent>();

                    if (collision && collision->isEnabled()) {
                        Vector2 collisionPos = collision->getCollisionWorldPosition();
                        Vector2 collisionSize = collision->getCollision().size;

                        Color red{1.0f, 0.0f, 0.0f, 1.0f}; // Red for collision
                        const CollisionShape* shape = collision->getCollisionShape();

                        if (shape) {
                            shape->renderGizmo(renderer, collisionPos, collisionSize, red);
                        }
                    }
                }

                // Draw texture bounds
                if (s_TextureBoundsEnabled) {
                    auto* texture = entity->getComponent<TextureComponent>();
        
                    if (texture && texture->hasTexture()) {
                        Color green{0.0f, 1.0f, 0.0f, 1.0f}; // Green for textures
                        texture->renderGizmo(renderer, position, size, green);
                    }
                }
            #endif
        }
    }
}