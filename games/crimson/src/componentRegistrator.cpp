#include <pch.h>

#include <Engine.h>

#include <componentRegistrator.h>

namespace crimson {
    void registrateComponents() {
        registerComponent<TransformComponent>("Transform");
        registerComponent<TextureComponent>("Texture");
        registerComponent<BoxRendererComponent>("BoxRenderer");
        registerComponent<InputComponent>("Input");
        registerComponent<CollisionComponent>("Collision");
    }
}
