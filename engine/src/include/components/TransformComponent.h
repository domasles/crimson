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

            void setPosition(const Vector2& position);
            void move(const Vector2& movement);

            void setRotation(float rotation);
            void rotate(float delta);

            void setSize(const Vector2& size);
            void addSize(const Vector2& delta);

            void setScale(const Vector2& scale);
            void addScale(const Vector2& delta);

            const Vector2& getPosition() const { return m_Transform.getPosition(); }
            const Vector2& getSize() const { return m_Transform.getSize(); }
            const Vector2& getScale() const { return m_Transform.getScale(); }

            float getRotation() const { return m_Transform.getRotation(); }
            void lookAt(const Vector2& target) { m_Transform.lookAt(target); }

            Transform& getTransform() { return m_Transform; }
            Vector2 getCenter() const { return m_Transform.getCenter(); }

            const Vector2& getPreviousPosition() const { return m_PreviousPosition; }
            const Vector2& getInterpolatedPosition() const { return m_InterpolatedPosition; }

            const Vector2& getPreviousSize() const { return m_PreviousSize; }
            const Vector2& getInterpolatedSize() const { return m_InterpolatedSize; }

            const Vector2& getPreviousScale() const { return m_PreviousScale; }
            const Vector2& getInterpolatedScale() const { return m_InterpolatedScale; }

            float getPreviousRotation() const { return m_PreviousRotation; }
            float getInterpolatedRotation() const { return m_InterpolatedRotation; }

            void setInterpolatedPosition(const Vector2& position) { m_InterpolatedPosition = position; }
            void setInterpolatedRotation(float rotation) { m_InterpolatedRotation = rotation; }
            void setInterpolatedSize(const Vector2& size) { m_InterpolatedSize = size; }
            void setInterpolatedScale(const Vector2& scale) { m_InterpolatedScale = scale; }

            void setParent(Entity* parent);
            bool hasParent() const { return m_Parent != nullptr; }
            void detach();

            Entity* getParent() const { return m_Parent; }
            const std::vector<Entity*>& getChildren() const { return m_Children; }

            float getWorldRotation() const;
            
            Vector2 getWorldPosition() const;
            Vector2 getWorldScale() const;

        private:
            Transform m_Transform;
            Entity* m_Parent = nullptr;
            std::vector<Entity*> m_Children;

            Vector2 m_PreviousPosition{ 0.0f, 0.0f };
            Vector2 m_InterpolatedPosition{ 0.0f, 0.0f };

            Vector2 m_PreviousSize{ 100.0f, 100.0f };
            Vector2 m_InterpolatedSize{ 100.0f, 100.0f };

            Vector2 m_PreviousScale{ 1.0f, 1.0f };
            Vector2 m_InterpolatedScale{ 1.0f, 1.0f };

            float m_PreviousRotation = 0.0f;
            float m_InterpolatedRotation = 0.0f;
    };
}
