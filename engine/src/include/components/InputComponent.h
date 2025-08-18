#pragma once

#include <utils/math.h>

#include <Component.h>
#include <Input.h>

using namespace engine::utils::math;

namespace engine {
    class InputComponent : public Component {
        public:
            InputComponent() = default;
            virtual ~InputComponent() = default;

            void setInputSystem(InputSystem* inputSystem) { m_InputSystem = inputSystem; }
            InputSystem* getInputSystem() const { return m_InputSystem; }

            Vector2 getMovementVector() const;
            bool isActionPressed(const std::string& action) const;

        private:
            InputSystem* m_InputSystem = nullptr;
    };
}
