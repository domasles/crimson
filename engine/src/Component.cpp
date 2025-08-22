#include <pch.h>

#include <utils/logger.h>

#include <Component.h>

using namespace engine::utils::logger;

namespace engine {
    ComponentManager& ComponentManager::getInstance() {
        try {
            static ComponentManager& instance = *new ComponentManager();
            return instance;
        }

        catch (const std::bad_alloc& e) {
            Logger::engine_error("Memory allocation failed: {}", e.what());
        }

        static ComponentManager fallbackInstance;
        return fallbackInstance;
    }
    
    std::unique_ptr<Component> ComponentManager::createComponent(const std::string& name) {
        auto it = m_ComponentFactories.find(name);

        if (it != m_ComponentFactories.end()) {
            return it->second();
        }

        Logger::engine_error("Component type '{}' not registered", name);
        return nullptr;
    }
}
