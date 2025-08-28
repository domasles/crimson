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

        Vector2 screenSize = getWindowSize();

        const float TOTAL_WIDTH = COLS * BRICK_WIDTH + (COLS - 1) * BRICK_SPACING;
        const float START_X = (screenSize.getRawX() - TOTAL_WIDTH) / 2.0f; // Center horizontally on actual screen
        const float START_Y = 100.0f; // Start from top with some margin

        for (int row = 0; row < ROWS; ++row) {
            for (int col = 0; col < COLS; ++col) {
                auto* brick = createEntity<Brick>();

                float x = START_X + col * (BRICK_WIDTH + BRICK_SPACING);
                float y = START_Y + row * (BRICK_HEIGHT + BRICK_SPACING);

                brick->init(Vector2{x, y});
                brick->setGridPosition(row, col);

                m_Bricks.push_back(brick);
            }
        }
    }
}
