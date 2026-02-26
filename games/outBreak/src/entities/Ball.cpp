#include <pch.h>

#include <entities/Ball.h>

using namespace engine::collisions::types;
using namespace engine::collisions::shapes;

namespace outBreak {
    void Ball::init() {
        auto* transform = addComponent<TransformComponent>();
        auto* texture = addComponent<TextureComponent>();
        auto* collision = addComponent<CollisionComponent>();

        texture->setTexture(loadTexture("assets/textures/ball.png"));

        transform->setSize({BALL_SIZE, BALL_SIZE});
        transform->setPosition(m_InitialPosition);

        collision->setCollisionType(std::make_unique<TriggerCollision>());
        collision->setCollisionShape(std::make_unique<CircleShape>());
        collision->setParticipatesInQueries(false);

        resetDirection();
    }

    void Ball::update(float deltaTime) {
        updateComponents(deltaTime);
        auto* transform = getComponent<TransformComponent>();

        if (transform) {
            Vector2 currentPos = transform->getPosition();
            Vector2 velocity = m_Direction * BALL_SPEED;
            Vector2 movement = velocity * deltaTime;

            float game_width = getLogicalWindowSize().getRawX();

            Vector2 nextPos = currentPos + movement;

            if (nextPos.getRawX() < 0.0f) {
                nextPos = Vector2{ 0.0f, nextPos.getRawY() };
                setDirectionX(1.0f);
            }
            else if (nextPos.getRawX() + BALL_SIZE > game_width) {
                nextPos = Vector2{ game_width - BALL_SIZE, nextPos.getRawY() };
                setDirectionX(-1.0f);
            }

            if (nextPos.getRawY() < 0.0f) {
                nextPos = Vector2{ nextPos.getRawX(), 0.0f };
                setDirectionY(1.0f);
            }

            transform->move(movement);
            transform->rotate(m_RotationSpeed * BALL_ROTATION_SPEED * deltaTime);
        }
    }

    void Ball::render() {
        auto* texture = getComponent<TextureComponent>();

        if (texture && texture->hasTexture()) {
            texture->render();
        }
    }

    void Ball::setDirectionX(float sign) { 
        float randomX = Random::getFloat(0.7f, 1.3f);
        float randomY = Random::getFloat(0.7f, 1.3f);
        float ySign = (m_Direction.getRawY() > 0) ? 1.0f : -1.0f;

        Vector2 newDirection{ sign * randomX, ySign * randomY };

        m_Direction = newDirection.normalize();
        m_RotationSpeed = Random::getFloat(-180.0f, 180.0f);
    }

    void Ball::setDirectionY(float sign) {
        float randomX = Random::getFloat(0.7f, 1.3f);
        float randomY = Random::getFloat(0.7f, 1.3f);
        float xSign = (m_Direction.getRawX() > 0) ? 1.0f : -1.0f;

        Vector2 newDirection{ xSign * randomX, sign * randomY };

        m_Direction = newDirection.normalize();
        m_RotationSpeed = Random::getFloat(-180.0f, 180.0f);
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
