#include <pch.h>

#include <utils/collision.h>

#include <collisions/shapes/BoxShape.h>

#include <Entity.h>

using namespace engine::collisions::shapes;
using namespace engine::collisions;

namespace engine::utils::collision {
    bool CollisionResult::isBlocking() const {
        return shouldBlock(hitType);
    }

    bool CollisionResult::isTrigger() const {
        return shouldTriggerEvents(hitType) && !shouldBlock(hitType);
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
        shape = std::make_unique<BoxShape>();
    }

    Collision::Collision(const Vector2& size) : size(size) {
        shape = std::make_unique<BoxShape>();
    }

    Collision::Collision(const Vector2& offset, const Vector2& size) : offset(offset), size(size) {
        shape = std::make_unique<BoxShape>();
    }

    Collision::Collision(const Collision& other) : type(other.type), offset(other.offset), size(other.size) {
        if (other.shape) shape = other.shape->clone();
    }

    Collision& Collision::operator=(const Collision& other) {
        if (this != &other) {
            type = other.type;
            offset = other.offset;
            size = other.size;

            if (other.shape) shape = other.shape->clone();
        }

        return *this;
    }
}
