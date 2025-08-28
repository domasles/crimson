#include <pch.h>

#include <scenes/MainScene.h>

#include <entities/GameManager.h>
#include <entities/Paddle.h>
#include <entities/Brick.h>
#include <entities/Ball.h>

namespace outBreak {
    void MainScene::init() {
        setInputSystem(std::make_unique<InputSystem>("config"));

        if (hasInputSystem()) {
            getInputSystem()->loadInputActions("InputActions.json");
        }

        m_Ball = createEntity<Ball>();
        m_Paddle = createEntity<Paddle>();
        m_GameManager = createEntity<GameManager>();

        m_Ball->init();
        m_Paddle->init();
        m_GameManager->init();

        createBricks();

        m_GameManager->setBall(m_Ball);
        m_GameManager->setPaddle(m_Paddle);

        for (auto* brick : m_Bricks) {
            m_GameManager->addBrick(brick);
        }
    }

    void MainScene::update(const float deltaTime) {
        updateEntities(deltaTime);
    }

    void MainScene::render() {
        renderEntities();
    }

    void MainScene::createBricks() {
        const int ROWS = 8;
        const int COLS = 18;
        const float BRICK_WIDTH = 80.0f;
        const float BRICK_HEIGHT = 30.0f;
        const float BRICK_SPACING = 5.0f;

        float totalWidth = COLS * BRICK_WIDTH + (COLS - 1) * BRICK_SPACING;

        for (int row = 0; row < ROWS; ++row) {
            for (int col = 0; col < COLS; ++col) {
                auto* brick = createEntity<Brick>();

                float x = col * (BRICK_WIDTH + BRICK_SPACING);
                float y = 100.0f + row * (BRICK_HEIGHT + BRICK_SPACING);

                brick->init(Vector2{x, y});
                brick->setGridPosition(row, col);

                // Store offset relative to grid center
                brick->setLocalOffset(Vector2(x - (totalWidth / 2.0f - BRICK_WIDTH / 2.0f), 0.0f));
                m_Bricks.push_back(brick);
            }
        }
    }
}
