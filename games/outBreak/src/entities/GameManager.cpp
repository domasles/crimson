#include <pch.h>

#include <entities/GameManager.h>
#include <entities/Ball.h>
#include <entities/Paddle.h>
#include <entities/Brick.h>

namespace outBreak {
    void GameManager::init() {
        // GameManager doesn't need visual components
    }

    void GameManager::update(float deltaTime) {
        updateComponents(deltaTime);
        
        checkCollisions();
        checkBallBounds();

        // Check win condition
        if (allBricksDestroyed()) {
            // Game won - reset everything
            resetGame();
        }
    }

    void GameManager::render() {
        // GameManager doesn't render anything visual
    }

    void GameManager::resetGame() {
        // Reset ball position and ensure predictable 45-degree angle
        if (m_Ball) {
            auto* transform = m_Ball->getComponent<TransformComponent>();
            if (transform) {
                transform->setPosition({800.0f - 8.0f, 400.0f}); // Center horizontally, middle vertically
            }
            // Use new resetDirection method to ensure predictable 45-degree movement
            m_Ball->resetDirection();
        }

        // Reset all bricks - make them alive again and reset all effects
        for (auto* brick : m_Bricks) {
            if (brick) {
                brick->setDestroyed(false); // Make brick alive again
                brick->setRainbowEffect(false);
                brick->setRainbowTime(0.0f);
                
                // Reset color to black
                auto* renderer = brick->getComponent<BoxRendererComponent>();
                if (renderer) {
                    renderer->setColor(Color(0.0f, 0.0f, 0.0f, 1.0f)); // Back to black
                }
            }
        }
    }

    void GameManager::checkCollisions() {
        checkBallPaddleCollision();
        checkBallBrickCollisions();
    }

    void GameManager::checkBallPaddleCollision() {
        if (!m_Ball || !m_Paddle) return;

        auto* ballTransform = m_Ball->getComponent<TransformComponent>();
        auto* paddleTransform = m_Paddle->getComponent<TransformComponent>();

        if (!ballTransform || !paddleTransform) return;

        Vector2 ballPos = ballTransform->getPosition();
        Vector2 ballSize = ballTransform->getSize();
        Vector2 paddlePos = paddleTransform->getPosition();
        Vector2 paddleSize = paddleTransform->getSize();

        // Expand collision area slightly to catch fast-moving balls
        float collisionMargin = 5.0f;
        
        // Check if ball is overlapping with paddle (with margin)
        if (ballPos.getRawX() < paddlePos.getRawX() + paddleSize.getRawX() + collisionMargin &&
            ballPos.getRawX() + ballSize.getRawX() > paddlePos.getRawX() - collisionMargin &&
            ballPos.getRawY() < paddlePos.getRawY() + paddleSize.getRawY() + collisionMargin &&
            ballPos.getRawY() + ballSize.getRawY() > paddlePos.getRawY() - collisionMargin) {
            
            // Get ball center and paddle center
            float ballCenterX = ballPos.getRawX() + ballSize.getRawX() / 2.0f;
            float ballCenterY = ballPos.getRawY() + ballSize.getRawY() / 2.0f;
            float paddleCenterX = paddlePos.getRawX() + paddleSize.getRawX() / 2.0f;
            float paddleCenterY = paddlePos.getRawY() + paddleSize.getRawY() / 2.0f;

            // Calculate distances to determine collision side
            float deltaX = ballCenterX - paddleCenterX;
            float deltaY = ballCenterY - paddleCenterY;
            
            // Calculate overlap on each axis
            float overlapX = (ballSize.getRawX() + paddleSize.getRawX()) / 2.0f - std::abs(deltaX);
            float overlapY = (ballSize.getRawY() + paddleSize.getRawY()) / 2.0f - std::abs(deltaY);

            // Collision response based on smallest overlap (most likely collision direction)
            if (overlapX < overlapY) {
                // Horizontal collision (ball hit paddle from side)
                m_Ball->reverseVelocityX();
                // Push ball away from paddle to prevent gluing (5px)
                if (deltaX > 0) {
                    ballPos = Vector2(paddlePos.getRawX() + paddleSize.getRawX() + 5.0f, ballPos.getRawY());
                } else {
                    ballPos = Vector2(paddlePos.getRawX() - ballSize.getRawX() - 5.0f, ballPos.getRawY());
                }
            } else {
                // Vertical collision (ball hit paddle from top/bottom)
                m_Ball->reverseVelocityY();
                // Push ball away from paddle to prevent gluing (5px)
                if (deltaY > 0) {
                    ballPos = Vector2(ballPos.getRawX(), paddlePos.getRawY() + paddleSize.getRawY() + 5.0f);
                } else {
                    ballPos = Vector2(ballPos.getRawX(), paddlePos.getRawY() - ballSize.getRawY() - 5.0f);
                }
            }
            
            // Update ball position to prevent getting stuck
            ballTransform->setPosition(ballPos);
        }
    }

    void GameManager::checkBallBrickCollisions() {
        if (!m_Ball) return;

        auto* ballTransform = m_Ball->getComponent<TransformComponent>();
        if (!ballTransform) return;

        Vector2 ballPos = ballTransform->getPosition();
        Vector2 ballSize = ballTransform->getSize();

        for (auto* brick : m_Bricks) {
            if (!brick || brick->isDestroyed()) continue;

            auto* brickTransform = brick->getComponent<TransformComponent>();
            if (!brickTransform) continue;

            Vector2 brickPos = brickTransform->getPosition();
            Vector2 brickSize = brickTransform->getSize();

            // AABB collision detection
            if (ballPos.getRawX() < brickPos.getRawX() + brickSize.getRawX() &&
                ballPos.getRawX() + ballSize.getRawX() > brickPos.getRawX() &&
                ballPos.getRawY() < brickPos.getRawY() + brickSize.getRawY() &&
                ballPos.getRawY() + ballSize.getRawY() > brickPos.getRawY()) {
                
                // Collision detected - destroy brick
                brick->setDestroyed(true);

                // Calculate collision side for proper bounce
                float ballCenterX = ballPos.getRawX() + ballSize.getRawX() / 2.0f;
                float ballCenterY = ballPos.getRawY() + ballSize.getRawY() / 2.0f;
                float brickCenterX = brickPos.getRawX() + brickSize.getRawX() / 2.0f;
                float brickCenterY = brickPos.getRawY() + brickSize.getRawY() / 2.0f;

                float deltaX = ballCenterX - brickCenterX;
                float deltaY = ballCenterY - brickCenterY;

                // Calculate overlap to determine collision direction more accurately
                float overlapX = (ballSize.getRawX() + brickSize.getRawX()) / 2.0f - std::abs(deltaX);
                float overlapY = (ballSize.getRawY() + brickSize.getRawY()) / 2.0f - std::abs(deltaY);

                // Respond based on smallest overlap (most likely collision direction)
                if (overlapX < overlapY) {
                    // Hit from side - reverse X direction
                    m_Ball->reverseVelocityX();
                    // Push ball away from brick to prevent tunneling (5px)
                    if (deltaX > 0) {
                        ballPos = Vector2(brickPos.getRawX() + brickSize.getRawX() + 5.0f, ballPos.getRawY());
                    } else {
                        ballPos = Vector2(brickPos.getRawX() - ballSize.getRawX() - 5.0f, ballPos.getRawY());
                    }
                } else {
                    // Hit from top/bottom - reverse Y direction
                    m_Ball->reverseVelocityY();
                    // Push ball away from brick to prevent tunneling (5px)
                    if (deltaY > 0) {
                        ballPos = Vector2(ballPos.getRawX(), brickPos.getRawY() + brickSize.getRawY() + 5.0f);
                    } else {
                        ballPos = Vector2(ballPos.getRawX(), brickPos.getRawY() - ballSize.getRawY() - 5.0f);
                    }
                }
                
                // Update ball position to prevent tunneling
                ballTransform->setPosition(ballPos);

                // Trigger wave effect from this brick's center position
                Vector2 brickCenter = brick->getPosition() + Vector2(25.0f, 15.0f); // Half brick size
                triggerWaveEffect(brickCenter);

                break; // Only handle one collision per frame
            }
        }
    }

    void GameManager::checkBallBounds() {
        if (!m_Ball) return;

        auto* ballTransform = m_Ball->getComponent<TransformComponent>();
        if (!ballTransform) return;

        Vector2 ballPos = ballTransform->getPosition();
        
        // Check if ball fell off bottom
        if (ballPos.getRawY() > 850.0f) {
            // Game over - reset everything
            resetGame();
        }
    }

    void GameManager::triggerRainbowEffect() {
        // Apply rainbow effect to all remaining bricks (old method - fallback)
        for (auto* brick : m_Bricks) {
            if (brick && !brick->isDestroyed()) {
                brick->setRainbowEffect(true);
                brick->setRainbowTime(0.0f);
            }
        }
    }

    void GameManager::triggerWaveEffect(Vector2 origin) {
        // Start wave effect from destroyed brick's position
        for (auto* brick : m_Bricks) {
            if (brick && !brick->isDestroyed()) {
                brick->addWaveFromOrigin(origin);
            }
        }
    }

    bool GameManager::allBricksDestroyed() {
        for (auto* brick : m_Bricks) {
            if (brick && !brick->isDestroyed()) {
                return false;
            }
        }
        return true;
    }
}
