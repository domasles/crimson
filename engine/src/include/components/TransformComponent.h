#pragma once

#include <Component.h>
#include <Transform.h>

#include <utils/math.h>

using namespace engine::utils::math;

namespace engine {
    class TransformComponent : public Component {
        public:
            TransformComponent() = default;
            virtual ~TransformComponent() = default;

            void setTransform(const Transform& transform) { m_Transform = transform; }

            void setPosition(const Vector2& position) { m_Transform.setPosition(position); }
            void addPosition(const Vector2& offset) { m_Transform.addPosition(offset); }

            void setSize(const Vector2& size) { m_Transform.setSize(size); }
            void setRotation(float rotation) { m_Transform.setRotation(rotation); }

            const Vector2& getPosition() const { return m_Transform.getPosition(); }
            const Vector2& getSize() const { return m_Transform.getSize(); }

            float getRotation() const { return m_Transform.getRotation(); }
            void lookAt(const Vector2& target) { m_Transform.lookAt(target); }

            Transform& getTransform() { return m_Transform; }
            Vector2 getCenter() const { return m_Transform.getCenter(); }

        private:
            Transform m_Transform;
    };
}
