#include <pch.h>

#include <scenes/Level1.h>

namespace crimson {
    void Level1::init() {
        setInputSystem(std::make_unique<InputSystem>("config"));

        if (hasInputSystem()) {
            getInputSystem()->loadInputActions("InputActions.json");
        }

        setMap(std::make_unique<Map>("assets/maps"));

        if (hasMap()) {
            getMap()->loadMap("TestMap.ldtk", {100, 100}, {0, 0});
        }

        m_Player = createEntity<Player>();
        m_Player->setInputSystem(getInputSystem());
        m_Player->init();

        if (hasMap()) {
            for (const auto& entity : getMap()->getEntitiesPositions()) {
                if (entity.first == "Player") {
                    m_Player->setPosition(entity.second);
                    break;
                }
            }
        }
    }

    void Level1::update(const float deltaTime) {
        updateEntities(deltaTime);
    }

    void Level1::render() {
        if (hasMap()) getMap()->render();
        renderEntities();
    }
}
