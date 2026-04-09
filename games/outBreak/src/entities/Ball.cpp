#include <pch.h>

#include <components/AudioSourceComponent.h>

#include <entities/Brick.h>
#include <entities/Ball.h>

using namespace engine::collisions;

namespace outBreak {
    void Ball::init() {
        auto* transform = addComponent<TransformComponent>();
        auto* texture = addComponent<TextureComponent>();
        auto* collision = addComponent<CollisionComponent>();
        auto* audio = addComponent<AudioSourceComponent>();

        texture->setTexture(loadTexture("assets/textures/ball.png"));
        audio->setSound(loadSound("assets/audio/sfx/bounce.mp3"));
        audio->setVolume(0.5f);

        transform->setSize({BALL_SIZE, BALL_SIZE});
        transform->setPosition(m_InitialPosition);

        collision->setCollisionType(CollisionType::Trigger);
        collision->setCollisionShape(std::make_unique<CircleShape>());
        collision->setParticipatesInQueries(false);

        resetDirection();
    }

    void Ball::playImpactSound() {
        if (auto* audio = getComponent<AudioSourceComponent>()) {
            audio->play();
        }
    }

    void Ball::reflectDirection(const Vector2& normal) {
        float randomX = Random::getFloat(0.7f, 1.3f);
        float randomY = Random::getFloat(0.7f, 1.3f);

        float xSign = (m_Direction.getRawX() > 0.0f) ? 1.0f : -1.0f;
        float ySign = (m_Direction.getRawY() > 0.0f) ? 1.0f : -1.0f;

        bool impactIsVertical = std::fabs(normal.getRawY()) >= std::fabs(normal.getRawX());

        float newX = impactIsVertical ? xSign * randomX : normal.getRawX() * randomX;
        float newY = impactIsVertical ? normal.getRawY() * randomY : ySign * randomY;

        m_Direction = Vector2{ newX, newY }.normalize();
        m_RotationSpeed = Random::getFloat(-180.0f, 180.0f);
    }

    bool Ball::handleWallCollision(Vector2& nextPos) {
        float game_width = getLogicalWindowSize().getRawX();

        if (nextPos.getRawX() < 0.0f) {
            nextPos = Vector2{ 0.0f, nextPos.getRawY() };

            reflectDirection(Vector2{1.0f, 0.0f});
            playImpactSound();

            return true;
        }

        if (nextPos.getRawX() + BALL_SIZE > game_width) {
            nextPos = Vector2{ game_width - BALL_SIZE, nextPos.getRawY() };

            reflectDirection(Vector2{-1.0f, 0.0f});
            playImpactSound();

            return true;
        }

        if (nextPos.getRawY() < 0.0f) {
            nextPos = Vector2{ nextPos.getRawX(), 0.0f };

            reflectDirection(Vector2{0.0f, 1.0f});
            playImpactSound();

            return true;
        }

        return false;
    }

    void Ball::handleCollisions(Vector2& nextPos) {
        if (handleWallCollision(nextPos)) return;

        auto* ballCollision = getComponent<CollisionComponent>();
        if (!ballCollision) return;

        MultiCollisionResult collisions = ballCollision->getAllCollisionsAt(nextPos);
        CollisionResult collision = collisions.getFirstBlocking();

        if (!collision.hasCollision) {
            m_LastCollisionEntity = nullptr;
            return;
        }

        bool shouldPlaySound = collision.hitEntity != m_LastCollisionEntity;
        m_LastCollisionEntity = collision.hitEntity;

        if (auto* brick = dynamic_cast<Brick*>(collision.hitEntity)) {
            brick->onHit(collision);
        }

        reflectDirection(collision.contactNormal);
        nextPos = nextPos + collision.contactNormal * 2.0f;
        if (shouldPlaySound) playImpactSound();
    }

    void Ball::update(float deltaTime) {
        updateComponents(deltaTime);
        auto* transform = getComponent<TransformComponent>();

        if (!transform) return;

        Vector2 currentPos = transform->getPosition();
        Vector2 velocity = m_Direction * BALL_SPEED;
        Vector2 movement = velocity * deltaTime;
        Vector2 nextPos = currentPos + movement;

        handleCollisions(nextPos);

        transform->setPosition(nextPos);
        transform->rotate(m_RotationSpeed * BALL_ROTATION_SPEED * deltaTime);
    }

    void Ball::render() {
        auto* texture = getComponent<TextureComponent>();

        if (texture && texture->hasTexture()) {
            texture->render();
        }
    }


    void Ball::resetDirection() { 
        float randomX = Random::getFloat(-1.0f, 1.0f);
        float randomY = Random::getFloat(-1.0f, -0.5f);

        m_Direction = Vector2{randomX, randomY}.normalize();
    }

    void Ball::resetPosition() {
        auto* transform = getComponent<TransformComponent>();

        if (transform) {
            m_LocalOffset = Vector2{ 0.0f, 0.0f };
            m_RotationSpeed = 0.0f;

            transform->setPosition(m_InitialPosition);
            transform->setRotation(0.0f);
        }
    }
}
