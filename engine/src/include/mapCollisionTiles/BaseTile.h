#pragma once

#include <Entity.h>

#include <components/TransformComponent.h>
#include <components/CollisionComponent.h>

namespace engine::mapCollisionTiles {
    class BaseTile : public Entity {
        public:
            BaseTile() = default;
            virtual ~BaseTile() = default;

            virtual void init() override;
            virtual void init(const Vector2& position);
            virtual void update(float deltaTime) override;
            virtual void render() override {}

            void setCollisionType(CollisionType newType);

        protected:
            virtual void configureCollision();
    };
}
