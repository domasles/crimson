#pragma once

#include <utils/logger.h>

#include <Component.h>

using namespace engine::utils::logger;

namespace engine {
    class ComponentManager {
        public:
            static ComponentManager& getInstance();

            template<typename T>

            void registerComponent(const std::string& name) {
                static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

                m_ComponentFactories[name] = []() -> std::unique_ptr<Component> {
                    return std::make_unique<T>();
                };

                ENGINE_LOG_INIT("Component registered: " + name);
            }

            std::unique_ptr<Component> createComponent(const std::string& name);

        private:
            ComponentManager() = default;
            ~ComponentManager() = default;

            ComponentManager(const ComponentManager&) = delete;
            ComponentManager& operator=(const ComponentManager&) = delete;

            std::unordered_map<std::string, std::function<std::unique_ptr<Component>()>> m_ComponentFactories;
    };

    inline ComponentManager& getComponentManager() {
        return ComponentManager::getInstance();
    }

    template<typename T>

    bool registerComponent(const std::string& name) {
        getComponentManager().registerComponent<T>(name);
        return true;
    }
}
