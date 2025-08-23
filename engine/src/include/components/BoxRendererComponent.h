#pragma once

#include <utils/rendering.h>
#include <utils/math.h>

#include <Component.h>

using namespace engine::utils::rendering;
using namespace engine::utils::math;

namespace engine {
    class BoxRendererComponent : public Component {
        public:
            BoxRendererComponent() = default;
            virtual ~BoxRendererComponent() = default;

            void setColor(const Color& color) { m_Color = color; }
            Color getColor() const { return m_Color; }

            void render();

        private:
            Color m_Color{ 0.0f, 0.0f, 0.0f, 1.0f };
    };
}
