#include <pch.h>

#include <Entity.h>

namespace engine {
    void Entity::updateComponents(float deltaTime) {
        for (auto& [type, component] : m_Components) {
            component->update(deltaTime);
        }
    }
}
