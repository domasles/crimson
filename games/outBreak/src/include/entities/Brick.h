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
            void init(const Vector2& position);
            void update(float deltaTime) override;
            void render() override;

            void setDestroyed(bool destroyed);
            bool isDestroyed() const { return m_IsDestroyed; }

            void setGridPosition(int row, int col) { m_GridRow = row; m_GridCol = col; }
            int getGridRow() const { return m_GridRow; }
            int getGridCol() const { return m_GridCol; }

            void setPosition(Vector2 position) { m_Position = position; }
            void addWaveFromOrigin(Vector2 origin);
            void setLocalOffset(const Vector2& offset) { m_LocalOffset = offset; }

            Vector2 getPosition() const { return m_Position; }

        private:
            struct WaveData {
                float distanceFromOrigin = 0.0f;
                float startTime = 0.0f;
            };

            bool m_IsDestroyed = false;

            int m_GridRow = 0;
            int m_GridCol = 0;

            float m_WaveSpeed = 300.0f;
            float m_WaveDuration = 1.0f;
            float m_GlobalTime = 0.0f;

            Vector2 m_Position{ 0.0f, 0.0f };
            Vector2 m_LocalOffset{ 0.0f, 0.0f };
            
            std::vector<WaveData> m_ActiveWaves;

            Color calculateWaveColor(float wavePhase);
            Color blendWaveColors(const std::vector<Color>& colors);
    };
}
