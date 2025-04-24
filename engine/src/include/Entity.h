#pragma once

#include <utils/math.h>

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

            void setPosition(Vector2 position) { m_Position = position; }
            void addPosition(Vector2 position) { m_Position += position; }

            const Vector2& getPosition() const { return m_Position; }

        protected:
            Vector2 m_Size = { 100.0f, 100.0f };
            Vector2 m_Position = { 0.0f, 0.0f };

            std::shared_ptr<Texture> m_Texture;
    };
}
