#include <pch.h>

#include <entities/Ball.h>

using namespace engine::collisions::types;
using namespace engine::collisions::shapes;

namespace outBreak {
    void Ball::init() {
        auto* transform = addComponent<TransformComponent>();
        auto* renderer = addComponent<BoxRendererComponent>();
        auto* collision = addComponent<CollisionComponent>();

        transform->setPosition({800.0f - BALL_SIZE/2, 700.0f});
        transform->setSize({BALL_SIZE, BALL_SIZE});
        renderer->setColor(Color(0.0f, 0.0f, 0.0f, 1.0f));

        collision->setCollisionType(std::make_unique<TriggerCollision>());
        collision->setCollisionShape(std::make_unique<BoxShape>());
        collision->setParticipatesInQueries(false);
    }

    void Ball::update(float deltaTime) {
        updateComponents(deltaTime);

        auto* transform = getComponent<TransformComponent>();
        if (!transform) return;

        Vector2 velocity = m_Direction * BALL_SPEED;
        Vector2 movement = velocity * deltaTime;

        transform->move(movement);

        Vector2 currentPos = transform->getPosition();
        Vector2 screenSize = getWindowSize();

        const float GAME_LEFT = 0.0f;   
        const float GAME_RIGHT = screenSize.getRawX(); 
        const float GAME_TOP = 0.0f;     

        if (currentPos.getRawX() <= GAME_LEFT || currentPos.getRawX() + BALL_SIZE >= GAME_RIGHT) {
            if (currentPos.getRawX() <= GAME_LEFT) {
                setDirectionX(1.0f);
            }
            
            else {
                setDirectionX(-1.0f);
            }

            Vector2 correctedPos(currentPos.getRawX() <= GAME_LEFT ? GAME_LEFT : GAME_RIGHT - BALL_SIZE, currentPos.getRawY());
            transform->setPosition(correctedPos);
        }

        if (currentPos.getRawY() <= GAME_TOP) {
            setDirectionY(1.0f);

            Vector2 correctedPos(currentPos.getRawX(), GAME_TOP);
            transform->setPosition(correctedPos);
        }
    }

    void Ball::render() {
        auto* renderer = getComponent<BoxRendererComponent>();

        if (renderer) {
            renderer->render();
        }
    }

    void Ball::setDirectionX(float sign) { 
        m_Direction = Vector2(sign * std::abs(m_Direction.getRawX()), m_Direction.getRawY()); 
    }

    void Ball::setDirectionY(float sign) { 
        m_Direction = Vector2(m_Direction.getRawX(), sign * std::abs(m_Direction.getRawY())); 
    }
}
