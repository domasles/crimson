#include <pch.h>

#include <entities/Camera.h>

namespace outBreak {
    void Camera::init() {
        auto* transform = addComponent<TransformComponent>();
        auto* camera = addComponent<CameraComponent>();

        transform->setPosition({800.0f, 450.0f});
        camera->setBackgroundColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
        camera->setPrimary();
    }
}
