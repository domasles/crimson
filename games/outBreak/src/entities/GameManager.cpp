#include <pch.h>

#include <components/AudioSourceComponent.h>

#include <entities/GameManager.h>
#include <entities/Brick.h>
#include <entities/Ball.h>

namespace outBreak {
    void GameManager::init() {
        auto* audio = addComponent<AudioSourceComponent>();

        audio->setSound(loadSound("assets/audio/music/bg-music.mp3"));
        audio->setVolume(0.3f);
        audio->setLoop(true);
        audio->setAutoPlay(true);
    }

    void GameManager::update(float deltaTime) {
        updateComponents(deltaTime);
        checkBallBounds();

        if (allBricksDestroyed()) {
            resetGame();
        }
    }

    void GameManager::onBrickHit(Brick* brick) {
        if (!brick || brick->isDestroyed()) return;

        brick->setDestroyed(true);
        Vector2 brickCenter = brick->getPosition() + Vector2{ 25.0f, 15.0f };
        triggerWaveEffect(brickCenter);
    }

    void GameManager::resetGame() {
        if (m_Ball) {
            auto* transform = m_Ball->getComponent<TransformComponent>();

            if (transform) {
                m_Ball->resetPosition();
                m_Ball->resetDirection();
            }
        }

        for (auto* brick : m_Bricks) {
            if (brick) {
                brick->setDestroyed(false);
                auto* renderer = brick->getComponent<BoxRendererComponent>();

                if (renderer) {
                    renderer->setColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
                }
            }
        }
    }

    void GameManager::checkBallBounds() {
        if (!m_Ball) return;

        auto* ballTransform = m_Ball->getComponent<TransformComponent>();
        if (!ballTransform) return;

        Vector2 ballPos = ballTransform->getPosition();

        if (ballPos.getRawY() > 900.0f) {
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
