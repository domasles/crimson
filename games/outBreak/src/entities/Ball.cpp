#include <pch.h>

#include <entities/Ball.h>

using namespace engine::collisions::types;
using namespace engine::collisions::shapes;

namespace outBreak {
    void Ball::init() {
        auto* transform = addComponent<TransformComponent>();
        auto* renderer = addComponent<BoxRendererComponent>();
        auto* collision = addComponent<CollisionComponent>();

        transform->setSize({BALL_SIZE, BALL_SIZE});
        transform->setPosition(m_InitialPosition);
        renderer->setColor(Color(0.0f, 0.0f, 0.0f, 1.0f));

        collision->setCollisionType(std::make_unique<TriggerCollision>());
        collision->setCollisionShape(std::make_unique<BoxShape>());
        collision->setParticipatesInQueries(false);
    }

    void Ball::update(float deltaTime) {
        updateComponents(deltaTime);

        auto* transform = getComponent<TransformComponent>();
        if (transform) {
            Vector2 currentPos = transform->getPosition();
            Vector2 velocity = m_Direction * BALL_SPEED;
            Vector2 movement = velocity * deltaTime;

            Vector2 screenSize = getWindowSize();
            Vector2 targetScreenSize = getTargetWindowSize();

            float game_width = getLogicalWindowSize().getRawX();

            m_LocalOffset += Vector2{ movement.getRawX(), 0.0f };

            float finalX = (game_width / 2.0f) - (BALL_SIZE / 2.0f) + m_LocalOffset.getRawX();
            float finalY = currentPos.getRawY() + movement.getRawY();

            if (finalX < 0.0f) {
                finalX = 0.0f;
                setDirectionX(1.0f);
                m_LocalOffset = Vector2{ finalX - (game_width / 2.0f - BALL_SIZE / 2.0f), m_LocalOffset.getRawY() };
            }

            else if (finalX + BALL_SIZE > game_width) {
                finalX = game_width - BALL_SIZE;
                setDirectionX(-1.0f);
                m_LocalOffset = Vector2{ finalX - (game_width / 2.0f - BALL_SIZE / 2.0f), m_LocalOffset.getRawY() };
            }

            if (finalY < 0.0f) {
                finalY = 0.0f;
                setDirectionY(1.0f);
            }

            transform->setPosition(Vector2{ finalX, finalY });
        }
    }

    void Ball::render() {
        auto* renderer = getComponent<BoxRendererComponent>();

        if (renderer) {
            renderer->render();
        }
    }

    void Ball::setDirectionX(float sign) { 
        m_Direction = Vector2{ sign * std::abs(m_Direction.getRawX()), m_Direction.getRawY() }; 
    }

    void Ball::setDirectionY(float sign) { 
        m_Direction = Vector2{ m_Direction.getRawX(), sign * std::abs(m_Direction.getRawY()) }; 
    }

    void Ball::resetPosition() {
        auto* transform = getComponent<TransformComponent>();

        if (transform) {
            m_LocalOffset = Vector2{ 0.0f, 0.0f };
            transform->setPosition(m_InitialPosition);
        }
    }
}
