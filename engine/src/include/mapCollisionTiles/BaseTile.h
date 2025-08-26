#pragma once

#include <Entity.h>

#include <components/TransformComponent.h>
#include <components/CollisionComponent.h>

#include <collisions/types/BlockCollision.h>
#include <collisions/shapes/BoxShape.h>

using namespace engine::collisions::types;
using namespace engine::collisions::shapes;

namespace engine::mapCollisionTiles {
    class BaseTile : public Entity {
        public:
            BaseTile() = default;
            virtual ~BaseTile() = default;

            virtual void init() override;
            virtual void init(const Vector2& position);
            virtual void update(float deltaTime) override;
            virtual void render() override {}

            void setCollisionType(std::unique_ptr<CollisionType> newType);

        protected:
            virtual void configureCollision();
    };
}
