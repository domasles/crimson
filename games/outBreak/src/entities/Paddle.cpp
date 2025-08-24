#include <pch.h>

#include <entities/Paddle.h>

namespace outBreak {
    void Paddle::init() {
        auto* transform = addComponent<TransformComponent>();
        auto* renderer = addComponent<BoxRendererComponent>();
        auto* input = addComponent<InputComponent>();
        auto* collision = addComponent<CollisionComponent>();

        // Set paddle size and position using logical coordinates
        // Paddle width in logical space (50 units in 1600px base = 0.03125 ratio)
        const float LOGICAL_PADDLE_WIDTH = 100.0f;
        const float LOGICAL_PADDLE_HEIGHT = 20.0f;
        
        transform->setSize({LOGICAL_PADDLE_WIDTH, LOGICAL_PADDLE_HEIGHT});
        
        // Position at center horizontally (800/1600 = 0.5), near bottom (780/900 = 0.867)
        // Using logical coordinates that work with Vector2 scaling
        transform->setPosition({800.0f - LOGICAL_PADDLE_WIDTH/2, 780.0f});

        renderer->setColor(Color(0.0f, 0.0f, 0.0f, 1.0f)); // Black

        // Set up input
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

            // Get current paddle width for proper bounds checking
            Vector2 paddleSize = transform->getSize();
            float paddleWidth = paddleSize.getRawX();

            // Use actual screen dimensions for boundaries
            Vector2 screenSize = getCore().getWindowSize();
            const float GAME_LEFT = 0.0f; // Left edge of screen
            const float GAME_RIGHT = screenSize.getRawX(); // Right edge of screen
            
            // Keep paddle within bounds using raw coordinates (logical space)
            if (newPos.getRawX() < GAME_LEFT) {
                newPos = Vector2(GAME_LEFT, newPos.getRawY());
            } else if (newPos.getRawX() + paddleWidth > GAME_RIGHT) {
                newPos = Vector2(GAME_RIGHT - paddleWidth, newPos.getRawY());
            }

            transform->setPosition(newPos);
        }
    }

    void Paddle::render() {
        // Call render on the BoxRendererComponent
        auto* renderer = getComponent<BoxRendererComponent>();
        if (renderer) {
            renderer->render();
        }
    }
}
