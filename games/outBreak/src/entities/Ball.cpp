#include <pch.h>

#include <entities/Ball.h>

namespace outBreak {
    void Ball::init() {
        auto* transform = addComponent<TransformComponent>();
        auto* renderer = addComponent<BoxRendererComponent>();

        transform->setPosition({800.0f - BALL_SIZE/2, 400.0f}); // Center horizontally, middle vertically
        transform->setSize({BALL_SIZE, BALL_SIZE});
        renderer->setColor(Color(0.0f, 0.0f, 0.0f, 1.0f)); // Black
    }

    void Ball::update(float deltaTime) {
        updateComponents(deltaTime);

        auto* transform = getComponent<TransformComponent>();
        if (!transform) return;

        // Use engine's TransformComponent movement system with normalized direction
        Vector2 velocity = m_Direction * BALL_SPEED;
        Vector2 movement = velocity * deltaTime;
        transform->move(movement); // Use engine's move() function instead of addPosition()

        // Get current position after engine movement
        Vector2 currentPos = transform->getPosition();

        // Handle screen boundaries
        Vector2 screenSize = getCore().getWindowSize();
        const float GAME_LEFT = 0.0f;   
        const float GAME_RIGHT = screenSize.getRawX(); 
        const float GAME_TOP = 0.0f;     

        // Boundary collision detection and response
        if (currentPos.getRawX() <= GAME_LEFT || currentPos.getRawX() + BALL_SIZE >= GAME_RIGHT) {
            reverseVelocityX();
            
            // Use engine's setPosition for correction
            Vector2 correctedPos(
                currentPos.getRawX() <= GAME_LEFT ? GAME_LEFT : GAME_RIGHT - BALL_SIZE,
                currentPos.getRawY()
            );
            transform->setPosition(correctedPos);
        }

        // Check top boundary
        if (currentPos.getRawY() <= GAME_TOP) {
            reverseVelocityY();
            
            // Use engine's setPosition for correction
            Vector2 correctedPos(currentPos.getRawX(), GAME_TOP);
            transform->setPosition(correctedPos);
        }

        // Ball position will be checked by GameManager for bottom boundary
    }

    void Ball::render() {
        // Call render on the BoxRendererComponent
        auto* renderer = getComponent<BoxRendererComponent>();
        if (renderer) {
            renderer->render();
        }
    }
}
