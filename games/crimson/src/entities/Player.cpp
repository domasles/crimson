#include <pch.h>

#include <entities/Player.h>

namespace crimson {
    void Player::init() {
        m_InputSystem = std::make_unique<InputSystem>("config");
        m_InputSystem->loadInputActions("InputActions.json");

        m_Texture = std::make_shared<Texture>("assets/tilesets");
        m_Texture->loadImage("TX Tileset Ground.png");

        m_Map = std::make_unique<Map>("assets/maps");
        m_Map->loadMap("TestMap.ldtk", { 100, 100 }, { 0, 0 });

        for (const auto& entity : m_Map->getEntitiesPositions()) {
            if (entity.first == "Player") {
                setPosition(entity.second);
                break;
            }
        }
    }

    void Player::update(float deltaTime) {
        Vector2 movement = m_InputSystem->getMovementVector();
        addPosition(movement * m_Speed * deltaTime);
    }

    void Player::render() {
        Vector2 cRegion{ 0, 0 };
        Vector2 cSize{ 16, 16 };

        if (!m_Texture->render(m_Size, m_Position, cSize, cRegion)) return;
    }
}
