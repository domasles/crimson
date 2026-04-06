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

        transform->setSize({PADDLE_WIDTH, PADDLE_HEIGHT});
        transform->setPosition({800.0f - PADDLE_WIDTH/2, 780.0f});

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
            Vector2 movementInput = input->getMovementVector();
            Vector2 currentPos = transform->getPosition();
            Vector2 paddleSize = transform->getSize();

            float paddleWidth = paddleSize.getRawX();
            float game_width = getLogicalWindowSize().getRawX();

            Vector2 movement = movementInput * m_Speed * deltaTime;
            Vector2 nextPos = currentPos + movement;

            if (nextPos.getRawX() < 0.0f) {
                nextPos = Vector2{ 0.0f, nextPos.getRawY() };
            }

            else if (nextPos.getRawX() + paddleWidth > game_width) {
                nextPos = Vector2{ game_width - paddleWidth, nextPos.getRawY() };
            }

            Vector2 actualMovement = nextPos - currentPos;
            transform->move(actualMovement);
        }
    }

    void Paddle::render() {
        auto* renderer = getComponent<BoxRendererComponent>();

        if (renderer) {
            renderer->render();
        }
    }
}
