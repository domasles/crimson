#include <Logic.h>

namespace crimson {
    void TestScene::init() {
        m_TestScene2 = std::make_shared<TestScene2>();
        m_TestTexture = utils::image::LoadImage(Game::getInstance().m_Renderer.get(), "assets/Square.png");
    }

    void TestScene::update(const float deltaTime) {
        Vector2 movement = Logic::getInstance().getInputSystem().getMovementVector();

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Current movement input vector: (%.2f, %.2f), change count: %d", movement.x, movement.y, changeCount);

        m_PlayerX += movement.x * m_Speed * deltaTime;
        m_PlayerY += movement.y * m_Speed * deltaTime;

        if (Logic::getInstance().getInputSystem().isActionPressed("shoot")) {
            changeCount += 1;
            SceneManager::getInstance().changeScene(m_TestScene2->getName());
        }
    }

    void TestScene::render() {
        if (!utils::image::RenderTexture(Game::getInstance().m_Renderer.get(), m_TestTexture.get(), m_PlayerX, m_PlayerY, 100, 100)) return;
    }

    void TestScene2::init() {
        m_TestScene = std::make_shared<TestScene>();
    }

    void TestScene2::update(const float deltaTime) {
        if (Logic::getInstance().getInputSystem().isActionPressed("move_up")) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Nice try switching me off!");
        }

        if (Logic::getInstance().getInputSystem().isActionPressed("move_down")) {
            SceneManager::getInstance().changeScene(m_TestScene->getName());
        }
    }

    void Logic::init() {
        m_InputSystem.loadInputActions("config/InputActions.json");

        m_TestScene = std::make_shared<TestScene>();
        m_TestScene2 = std::make_shared<TestScene2>();

        if (!SceneManager::getInstance().registerScene(m_TestScene->getName(), m_TestScene)) return;
        if (!SceneManager::getInstance().registerScene(m_TestScene2->getName(), m_TestScene2)) return;
        if (!SceneManager::getInstance().changeScene(m_TestScene->getName())) return;
    }

    void Logic::update() {
        SceneManager::getInstance().update();
    }

    void Logic::render() {
        SceneManager::getInstance().render();
    }
}
