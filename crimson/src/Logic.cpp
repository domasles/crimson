#include <Logic.h>

namespace crimson {
    void Logic::init() {
        m_InputSystem.loadInputActions("config/InputActions.json");

        m_TestScene = std::make_shared<TestScene>();
        m_TestScene2 = std::make_shared<TestScene2>();

        if (!SceneManager::getInstance().registerScene(m_TestScene->getName(), m_TestScene)) return;
        if (!SceneManager::getInstance().registerScene(m_TestScene2->getName(), m_TestScene2)) return;
        if (!SceneManager::getInstance().changeScene(m_TestScene->getName())) return;
    }

    void TestScene::update(float deltaTime) {
        Vector2 movement = Logic::getInstance().getInputSystem().getMovementVector();

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Current movement input vector: (%.2f, %.2f), change count: %d", movement.x, movement.y, changeCount);

        if (Logic::getInstance().getInputSystem().isActionPressed("shoot")) {
            changeCount += 1;
            SceneManager::getInstance().changeScene(m_TestScene2->getName());
        }
    }

    void TestScene2::update(float deltaTime) {
        if (Logic::getInstance().getInputSystem().isActionPressed("move_up")) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Nice try switching me off!");
        }

        if (Logic::getInstance().getInputSystem().isActionPressed("move_down")) {
            SceneManager::getInstance().changeScene(m_TestScene->getName());
        }
    }
}
