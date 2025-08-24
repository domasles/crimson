#include <pch.h>

#include <scenes/MainScene.h>
#include <entities/Ball.h>
#include <entities/Paddle.h>
#include <entities/Brick.h>
#include <entities/GameManager.h>

namespace outBreak {
    void MainScene::init() {
        setInputSystem(std::make_unique<InputSystem>("config"));

        if (hasInputSystem()) {
            getInputSystem()->loadInputActions("InputActions.json");
        }

        // Create game entities using the engine's createEntity method
        m_Ball = createEntity<Ball>();
        m_Paddle = createEntity<Paddle>();
        m_GameManager = createEntity<GameManager>();

        // Initialize entities
        m_Ball->init();
        m_Paddle->init();
        m_GameManager->init();

        // Create brick grid
        createBricks();

        // Set up game manager references
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
        const int COLS = 18; // Increased from 12 to 18 for more blocks to the sides
        const float BRICK_WIDTH = 80.0f;
        const float BRICK_HEIGHT = 30.0f;
        const float BRICK_SPACING = 5.0f;
        
        // Get actual screen dimensions and calculate starting position to center the brick grid
        Vector2 screenSize = getCore().getWindowSize();
        const float TOTAL_WIDTH = COLS * BRICK_WIDTH + (COLS - 1) * BRICK_SPACING;
        const float START_X = (screenSize.getRawX() - TOTAL_WIDTH) / 2.0f; // Center horizontally on actual screen
        const float START_Y = 100.0f; // Start from top with some margin

        for (int row = 0; row < ROWS; ++row) {
            for (int col = 0; col < COLS; ++col) {
                auto* brick = createEntity<Brick>();
                brick->init();
                
                // Set grid position for wave calculations
                brick->setGridPosition(row, col);
                
                auto* transform = brick->getComponent<TransformComponent>();
                if (transform) {
                    float x = START_X + col * (BRICK_WIDTH + BRICK_SPACING);
                    float y = START_Y + row * (BRICK_HEIGHT + BRICK_SPACING);
                    transform->setPosition({x, y});
                    brick->setPosition({x, y}); // Store position for wave calculations
                }
                
                m_Bricks.push_back(brick);
            }
        }
    }
}
