#include <pch.h>

#include <Logic.h>
#include <Game.h>

namespace crimson {
    void TestScene::init() {
        m_Player = std::make_shared<Player>();
        m_TestScene2 = std::make_shared<TestScene2>();

        m_Player->init();
        m_Player->setPosition(m_Position);
    }

    void TestScene::update(const float deltaTime) {
        Vector2 movement = Logic::getInstance().getInputSystem().getMovementVector();

        m_Player->addPosition(movement * m_Speed * deltaTime);

        if (Logic::getInstance().getInputSystem().isActionPressed("shoot")) {
            m_ChangeCount += 1;
            SceneManager::getInstance().changeScene(m_TestScene2->getName());
        }
    }

    void TestScene::render() {
        m_Player->render();
    }

    void TestScene2::init() {
        m_TestScene = std::make_shared<TestScene>();
    }

    void TestScene2::update(const float deltaTime) {
        if (Logic::getInstance().getInputSystem().isActionPressed("move_down")) {
            SceneManager::getInstance().changeScene(m_TestScene->getName());
        }
    }

    void Logic::init() {
        m_InputSystem.loadInputActions("config/InputActions.json");

        m_TestScene = std::make_shared<TestScene>();
        m_TestScene2 = std::make_shared<TestScene2>();

        if (!SceneManager::getInstance().registerScene(m_TestScene->getName(), m_TestScene)) {
            return;
        }

        if (!SceneManager::getInstance().registerScene(m_TestScene2->getName(), m_TestScene2)){
            return;
        }

        if (!SceneManager::getInstance().changeScene(m_TestScene->getName())) {
            return;
        }
    }

    void Logic::update() {
        SceneManager::getInstance().update();
    }

    void Logic::render() {
        SceneManager::getInstance().render();
    }
}
