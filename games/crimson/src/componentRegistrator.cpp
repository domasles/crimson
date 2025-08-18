#include <pch.h>

#include <components/TransformComponent.h>
#include <components/TextureComponent.h>
#include <components/InputComponent.h>

#include <ComponentManager.h>

#include <componentRegistrator.h>

namespace crimson {
    void registrateComponents() {
        engine::registerComponent<engine::TransformComponent>("Transform");
        engine::registerComponent<engine::TextureComponent>("Texture");
        engine::registerComponent<engine::InputComponent>("Input");
    }
}
