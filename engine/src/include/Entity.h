#pragma once

#include <typeindex>

#include <Component.h>

namespace engine {
    class Entity {
        public:
            virtual ~Entity() = default;

            virtual void init() = 0;
            virtual void update(const float deltaTime) = 0;
            virtual void render() = 0;

            template<typename T, typename... Args>

            T* addComponent(Args&&... args) {
                static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
                auto component = std::make_unique<T>(std::forward<Args>(args)...);

                T* ptr = component.get();

                component->setEntity(this);
                component->init();
                m_Components[std::type_index(typeid(T))] = std::move(component);

                return ptr;
            }

            template<typename T>

            T* getComponent() {
                static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
                auto it = m_Components.find(std::type_index(typeid(T)));

                if (it != m_Components.end()) {
                    return static_cast<T*>(it->second.get());
                }
                return nullptr;
            }

            template<typename T>

            bool hasComponent() const {
                return m_Components.find(std::type_index(typeid(T))) != m_Components.end();
            }
            
            template<typename T>

            void removeComponent() {
                auto it = m_Components.find(std::type_index(typeid(T)));

                if (it != m_Components.end()) {
                    it->second->onDestroy();
                    m_Components.erase(it);
                }
            }

        protected:
            std::unordered_map<std::type_index, std::unique_ptr<Component>> m_Components;

            void updateComponents(float deltaTime);
    };
}
