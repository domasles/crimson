#include <pch.h>

#include <utils/collision.h>

#include <collisions/types/BlockCollision.h>
#include <collisions/shapes/BoxShape.h>

#include <Entity.h>

using namespace engine::collisions::shapes;
using namespace engine::collisions::types;

namespace engine::utils::collision {
    bool CollisionResult::isBlocking() const {
        return hitType && hitType->shouldBlock();
    }

    bool CollisionResult::isTrigger() const {
        return hitType && hitType->shouldTriggerEvents() && !hitType->shouldBlock();
    }

    bool MultiCollisionResult::hasBlockingCollision() const {
        for (const auto& collision : collisions) {
            if (collision.isBlocking()) return true;
        }

        return false;
    }

    CollisionResult MultiCollisionResult::getFirstBlocking() const {
        for (const auto& collision : collisions) {
            if (collision.isBlocking()) return collision;
        }

        return CollisionResult{};
    }

    std::vector<CollisionResult> MultiCollisionResult::getAllBlocking() const {
        std::vector<CollisionResult> blocking;

        for (const auto& collision : collisions) {
            if (collision.isBlocking()) blocking.push_back(collision);
        }

        return blocking;
    }

    std::vector<CollisionResult> MultiCollisionResult::getAllTriggers() const {
        std::vector<CollisionResult> triggers;

        for (const auto& collision : collisions) {
            if (collision.isTrigger()) triggers.push_back(collision);
        }

        return triggers;
    }

    Collision::Collision() {
        type = std::make_unique<BlockCollision>();
        shape = std::make_unique<BoxShape>();
    }

    Collision::Collision(const Vector2& size) : size(size) {
        type = std::make_unique<BlockCollision>();
        shape = std::make_unique<BoxShape>();
    }

    Collision::Collision(const Vector2& offset, const Vector2& size) : offset(offset), size(size) {
        type = std::make_unique<BlockCollision>();
        shape = std::make_unique<BoxShape>();
    }

    Collision::Collision(const Collision& other) : offset(other.offset), size(other.size) {
        if (other.type) type = other.type->clone();
        if (other.shape) shape = other.shape->clone();
    }

    Collision& Collision::operator=(const Collision& other) {
        if (this != &other) {
            offset = other.offset;
            size = other.size;

            if (other.type) type = other.type->clone();
            if (other.shape) shape = other.shape->clone();
        }

        return *this;
    }
}
