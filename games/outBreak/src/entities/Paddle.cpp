#include <pch.h>

#include <entities/Paddle.h>

using namespace engine::collisions::types;
using namespace engine::collisions::shapes;

namespace outBreak {
    void Paddle::init() {
        auto* transform = addComponent<TransformComponent>();
        auto* renderer = addComponent<BoxRendererComponent>();
        auto* input = addComponent<InputComponent>();
        auto* collision = addComponent<CollisionComponent>();

        const float LOGICAL_PADDLE_WIDTH = 100.0f;
        const float LOGICAL_PADDLE_HEIGHT = 20.0f;
        
        transform->setSize({LOGICAL_PADDLE_WIDTH, LOGICAL_PADDLE_HEIGHT});
        transform->setPosition({800.0f - LOGICAL_PADDLE_WIDTH/2, 780.0f});

        renderer->setColor(Color(0.0f, 0.0f, 0.0f, 1.0f));

        collision->setCollisionType(std::make_unique<BlockCollision>());
        collision->setCollisionShape(std::make_unique<BoxShape>());
        collision->setParticipatesInQueries(true);

        if (getSceneManager().getCurrentScene()->hasInputSystem()) {
            input->setInputSystem(getSceneManager().getCurrentScene()->getInputSystem());
        }
    }

    void Paddle::update(float deltaTime) {
        updateComponents(deltaTime);

        auto* input = getComponent<InputComponent>();
        auto* transform = getComponent<TransformComponent>();

        if (input && transform && input->getInputSystem()) {
            Vector2 movement = input->getMovementVector();
            Vector2 currentPos = transform->getPosition();
            Vector2 newPos = currentPos + (movement * m_Speed * deltaTime);

            Vector2 paddleSize = transform->getSize();
            float paddleWidth = paddleSize.getRawX();

            Vector2 screenSize = getWindowSize();

            const float GAME_LEFT = 0.0f;
            const float GAME_RIGHT = screenSize.getRawX();

            if (newPos.getRawX() < GAME_LEFT) {
                newPos = Vector2(GAME_LEFT, newPos.getRawY());
            }

            else if (newPos.getRawX() + paddleWidth > GAME_RIGHT) {
                newPos = Vector2(GAME_RIGHT - paddleWidth, newPos.getRawY());
            }

            transform->setPosition(newPos);
        }
    }

    void Paddle::render() {
        auto* renderer = getComponent<BoxRendererComponent>();

        if (renderer) {
            renderer->render();
        }
    }
}
