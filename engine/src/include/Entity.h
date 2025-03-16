#pragma once

#include <utils/math.h>
#include <utils/image.h>

#include <Core.h>

using namespace engine::utils::math;

namespace engine {
    class Entity {
        public:
            virtual void init() = 0;
            virtual void update(const float deltaTime) = 0;
            virtual void render() = 0;

            void setPosition(Vector2 position) { m_Position = position; }
            void addPosition(Vector2 position) { m_Position += position; }

            const Vector2& getPosition() const { return m_Position; }

        protected:
            Entity() = default;
            ~Entity() = default;

            Core& m_EngineCore = Core::getInstance();

            Vector2 m_Position{ 0, 0 };
            Vector2 m_Stretch{ 0, 0 };
    };
}
