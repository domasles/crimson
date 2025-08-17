#pragma once

#include <utils/math.h>

#include <Transform.h>
#include <Texture.h>
#include <Core.h>

using namespace engine::utils::math;

namespace engine {
    class Entity {
        public:
            virtual ~Entity() = default;

            virtual void init() = 0;
            virtual void update(const float deltaTime) = 0;
            virtual void render() = 0;

            void setTransform(const Transform& transform) { m_Transform = transform; }

            Transform& getTransform() { return m_Transform; }

            void setPosition(const Vector2& position) { m_Transform.setPosition(position); }
            void addPosition(const Vector2& offset) { m_Transform.addPosition(offset); }

            void setSize(const Vector2& size) { m_Transform.setSize(size); }

            const Vector2& getPosition() const { return m_Transform.getPosition(); }
            const Vector2& getSize() const { return m_Transform.getSize(); }

            void setInputSystem(InputSystem* inputSystem) { m_InputSystem = inputSystem; }

            void setRotation(float rotation) { m_Transform.setRotation(rotation); }
            float getRotation() const { return m_Transform.getRotation(); }

            void setTexture(std::shared_ptr<Texture> texture) { m_Texture = texture; }
            bool hasTexture() const { return m_Texture != nullptr; }

            std::shared_ptr<Texture> getTexture() const { return m_Texture; }

        protected:
            Transform m_Transform;
            InputSystem* m_InputSystem = nullptr;
            std::shared_ptr<Texture> m_Texture = nullptr;
    };
}
