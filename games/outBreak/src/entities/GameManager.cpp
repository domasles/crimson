#include <pch.h>

#include <entities/GameManager.h>
#include <entities/Paddle.h>
#include <entities/Brick.h>
#include <entities/Ball.h>

using namespace engine::utils::collision;

namespace outBreak {
    void GameManager::update(float deltaTime) {
        updateComponents(deltaTime);
        
        checkCollisions();
        checkBallBounds();

        if (allBricksDestroyed()) {
            resetGame();
        }
    }

    void GameManager::resetGame() {
        if (m_Ball) {
            auto* transform = m_Ball->getComponent<TransformComponent>();

            if (transform) {
                transform->setPosition(m_Ball->getInitialPosition());
            }

            m_Ball->resetDirection();
        }

        for (auto* brick : m_Bricks) {
            if (brick) {
                brick->setDestroyed(false); // Make brick alive again
                auto* renderer = brick->getComponent<BoxRendererComponent>();

                if (renderer) {
                    renderer->setColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
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

        auto* ballCollision = m_Ball->getComponent<CollisionComponent>();
        auto* ballTransform = m_Ball->getComponent<TransformComponent>();
        
        if (!ballCollision || !ballTransform) return;

        CollisionResult collision = ballCollision->checkCollisionWithEntity(m_Paddle);
        
        if (collision.hasCollision) {
            Vector2 ballPos = ballTransform->getPosition();
            Vector2 ballSize = ballTransform->getSize();
            
            auto* paddleTransform = m_Paddle->getComponent<TransformComponent>();

            Vector2 paddlePos = paddleTransform->getPosition();
            Vector2 paddleSize = paddleTransform->getSize();

            Vector2 normal = collision.contactNormal;

            if (std::abs(normal.getRawX()) > std::abs(normal.getRawY())) {
                m_Ball->setDirectionX(normal.getRawX() > 0 ? 1.0f : -1.0f);

                if (normal.getRawX() > 0) {
                    ballPos = Vector2(paddlePos.getRawX() + paddleSize.getRawX() + 5.0f, ballPos.getRawY());
                }

                else {
                    ballPos = Vector2(paddlePos.getRawX() - ballSize.getRawX() - 5.0f, ballPos.getRawY());
                }
            }

            else {
                m_Ball->setDirectionY(normal.getRawY() > 0 ? 1.0f : -1.0f);

                if (normal.getRawY() > 0) {
                    ballPos = Vector2(ballPos.getRawX(), paddlePos.getRawY() + paddleSize.getRawY() + 5.0f);
                }

                else {
                    ballPos = Vector2(ballPos.getRawX(), paddlePos.getRawY() - ballSize.getRawY() - 5.0f);
                }
            }

            ballTransform->setPosition(ballPos);
        }
    }

    void GameManager::checkBallBrickCollisions() {
        if (!m_Ball) return;

        auto* ballCollision = m_Ball->getComponent<CollisionComponent>();
        auto* ballTransform = m_Ball->getComponent<TransformComponent>();

        if (!ballCollision || !ballTransform) return;

        Vector2 ballPos = ballTransform->getPosition();
        Vector2 ballSize = ballTransform->getSize();

        for (auto* brick : m_Bricks) {
            if (!brick || brick->isDestroyed()) continue;

            CollisionResult collision = ballCollision->checkCollisionWithEntity(brick);
            
            if (collision.hasCollision) {
                brick->setDestroyed(true);

                auto* brickTransform = brick->getComponent<TransformComponent>();

                Vector2 brickPos = brickTransform->getPosition();
                Vector2 brickSize = brickTransform->getSize();
                Vector2 normal = collision.contactNormal;
                
                // Set absolute direction based on collision normal
                if (std::abs(normal.getRawX()) > std::abs(normal.getRawY())) {
                    // Horizontal collision - set X direction based on normal
                    m_Ball->setDirectionX(normal.getRawX() > 0 ? 1.0f : -1.0f);

                    if (normal.getRawX() > 0) {
                        ballPos = Vector2(brickPos.getRawX() + brickSize.getRawX() + 5.0f, ballPos.getRawY());
                    }

                    else {
                        ballPos = Vector2(brickPos.getRawX() - ballSize.getRawX() - 5.0f, ballPos.getRawY());
                    }
                }

                else {
                    // Vertical collision - set Y direction based on normal
                    m_Ball->setDirectionY(normal.getRawY() > 0 ? 1.0f : -1.0f);

                    if (normal.getRawY() > 0) {
                        ballPos = Vector2(ballPos.getRawX(), brickPos.getRawY() + brickSize.getRawY() + 5.0f);
                    }

                    else {
                        ballPos = Vector2(ballPos.getRawX(), brickPos.getRawY() - ballSize.getRawY() - 5.0f);
                    }
                }

                ballTransform->setPosition(ballPos);

                // Trigger wave effect from this brick's center position
                Vector2 brickCenter = brick->getPosition() + Vector2(25.0f, 15.0f);
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

        if (ballPos.getRawY() > 850.0f) {
            resetGame();
        }
    }

    void GameManager::triggerWaveEffect(Vector2 origin) {
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
