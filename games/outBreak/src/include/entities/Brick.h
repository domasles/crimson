#pragma once

#include <Engine.h>
#include <vector>

using namespace engine;

namespace outBreak {
    class Brick : public Entity {
        public:
            Brick() = default;
            virtual ~Brick() = default;

            void init() override;
            void init(const Vector2& position); // New init with position parameter
            void update(float deltaTime) override;
            void render() override;

            void setDestroyed(bool destroyed) { m_IsDestroyed = destroyed; }
            bool isDestroyed() const { return m_IsDestroyed; }

            void setRainbowEffect(bool enabled) { m_RainbowEffect = enabled; }
            void setRainbowTime(float time) { m_RainbowTime = time; }

            // Grid position for wave calculations
            void setGridPosition(int row, int col) { m_GridRow = row; m_GridCol = col; }
            int getGridRow() const { return m_GridRow; }
            int getGridCol() const { return m_GridCol; }

            // Position access
            Vector2 getPosition() const { return m_Position; }
            void setPosition(Vector2 position) { m_Position = position; }

            // Wave effect - support multiple simultaneous waves
            void addWaveFromOrigin(Vector2 origin);

        private:
            // Wave data for tracking multiple simultaneous waves
            struct WaveData {
                float distanceFromOrigin = 0.0f;
                float startTime = 0.0f;
            };

            bool m_IsDestroyed = false;
            Vector2 m_Position = Vector2(0.0f, 0.0f);
            
            // Grid position
            int m_GridRow = 0;
            int m_GridCol = 0;

            // Multiple wave effects system
            std::vector<WaveData> m_ActiveWaves;
            float m_WaveSpeed = 300.0f; // Wave speed (pixels per second)
            float m_WaveDuration = 1.0f; // How long wave effect lasts on each brick
            float m_GlobalTime = 0.0f; // Time tracking for wave calculations

            // Rainbow effect (legacy - for fallback)
            bool m_RainbowEffect = false;
            float m_RainbowTime = 0.0f;
            float m_RainbowDuration = 3.0f;

            // Wave color calculation
            Color calculateWaveColor(float wavePhase);
            Color blendWaveColors(const std::vector<Color>& colors);
            Color calculateRainbowColor(float time);
    };
}
