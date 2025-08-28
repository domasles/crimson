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
            Vector2 movement = input->getMovementVector();
            Vector2 currentPos = transform->getPosition();

            Vector2 paddleSize = transform->getSize();
            float paddleWidth = paddleSize.getRawX();

            Vector2 screenSize = getWindowSize();
            Vector2 targetScreenSize = getTargetWindowSize();

            float game_width = getLogicalWindowSize().getRawX();

            m_LocalOffset += movement * m_Speed * deltaTime;

            float finalX = (game_width / 2.0f) - (paddleWidth / 2.0f) + m_LocalOffset.getRawX();
            float finalY = currentPos.getRawY();

            if (finalX < 0.0f) {
                finalX = 0.0f;
                m_LocalOffset = Vector2(finalX - (game_width / 2.0f - paddleWidth / 2.0f), m_LocalOffset.getRawY());
            }

            else if (finalX + paddleWidth > game_width) {
                finalX = game_width - paddleWidth;
                m_LocalOffset = Vector2(finalX - (game_width / 2.0f - paddleWidth / 2.0f), m_LocalOffset.getRawY());
            }

            transform->setPosition(Vector2(finalX, finalY));
        }
    }

    void Paddle::render() {
        auto* renderer = getComponent<BoxRendererComponent>();

        if (renderer) {
            renderer->render();
        }
    }
}
