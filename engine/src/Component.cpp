#include <pch.h>

#include <utils/singleton.h>
#include <utils/logger.h>

#include <Component.h>

using namespace engine::utils::logger;
using namespace engine::utils;

namespace engine {
    ComponentManager& ComponentManager::getInstance() {
        return singleton<ComponentManager>();
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
