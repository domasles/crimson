#include <pch.h>

#include <Entity.h>

namespace engine {
    Entity::~Entity() {
        for (auto& [type, component] : m_Components) {
            if (component) component->onDestroy();
        }
    }

    void Entity::updateComponents(float deltaTime) {
        for (auto& [type, component] : m_Components) {
            component->update(deltaTime);
        }
    }
}
