#pragma once

#include <Component.h>
#include <utils/math.h>
#include <utils/rendering.h>

using namespace engine::utils::math;
using namespace engine::utils::rendering;

namespace engine {
    class CameraComponent : public Component {
        public:
            CameraComponent() = default;
            ~CameraComponent() override = default;

            void setPrimary();

            void setZoom(float zoom) { m_Zoom = std::max(0.01f, zoom); }
            float getZoom() const { return m_Zoom; }

            void setBackgroundColor(const Color& color) { m_BackgroundColor = color; }
            const Color& getBackgroundColor() const { return m_BackgroundColor; }

            Vector2 screenToWorld(const Vector2& screenPos, float viewW, float viewH) const;

        private:
            float m_Zoom = 1.0f;
            Color m_BackgroundColor{ 0.0f, 0.0f, 0.0f, 1.0f };
    };
}
