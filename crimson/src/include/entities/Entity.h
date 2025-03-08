#pragma once

#include <utils/math.h>
#include <utils/image.h>

using namespace crimson::utils::math;

namespace crimson {
    class Entity {
        public:
            virtual void init() = 0;
            virtual void update(const float deltaTime) = 0;
            virtual void render() = 0;

            void setPosition(Vector2 position) { m_Position = position; }
            void addPosition(Vector2 position) { m_Position += position; }

            Vector2 getPosition() const { return m_Position; };

        protected:
            Entity() : m_Position({ 0, 0 }) {}
            ~Entity() {}

            Vector2 m_Position;
            Vector2 m_Stretch;
    };
}
