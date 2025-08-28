#include <pch.h>

#include <entities/Brick.h>

using namespace engine::collisions::types;
using namespace engine::collisions::shapes;

namespace outBreak {
    const float BRICK_WIDTH = 80.0f;
    const float BRICK_HEIGHT = 30.0f;

    void Brick::init() {
        init(Vector2{0.0f, 0.0f});
    }

    void Brick::init(const Vector2& position) {
        auto* transform = addComponent<TransformComponent>();
        auto* renderer = addComponent<BoxRendererComponent>();
        auto* collision = addComponent<CollisionComponent>();

        transform->setPosition(position);
        transform->setSize({BRICK_WIDTH, BRICK_HEIGHT});
        renderer->setColor(Color(0.0f, 0.0f, 0.0f, 1.0f));

        collision->setCollisionType(std::make_unique<BlockCollision>());
        collision->setCollisionShape(std::make_unique<BoxShape>());
        collision->setParticipatesInQueries(true);

        m_Position = position;
    }

    void Brick::update(float deltaTime) {
        updateComponents(deltaTime);
        m_GlobalTime += deltaTime;

        auto* renderer = getComponent<BoxRendererComponent>();

        if (!renderer) return;
        if (m_IsDestroyed) return;

        auto* transform = getComponent<TransformComponent>();

        if (transform) {
            Vector2 brickSize = transform->getSize();
            float brickWidth = brickSize.getRawX();

            float game_width = getLogicalWindowSize().getRawX();
            Vector2 currentPos = transform->getPosition();

            float finalX = (game_width / 2.0f) - (brickWidth / 2.0f) + m_LocalOffset.getRawX();
            float finalY = currentPos.getRawY();

            transform->setPosition(Vector2{ finalX, finalY });
        }

        std::vector<Color> waveColors;

        for (auto it = m_ActiveWaves.begin(); it != m_ActiveWaves.end();) {
            WaveData& wave = *it;

            float waveArrivalTime = wave.startTime + (wave.distanceFromOrigin / m_WaveSpeed);
            float timeSinceWaveArrival = m_GlobalTime - waveArrivalTime;

            if (timeSinceWaveArrival >= 0.0f && timeSinceWaveArrival < m_WaveDuration) {
                float wavePhase = timeSinceWaveArrival / m_WaveDuration;
                waveColors.push_back(calculateWaveColor(wavePhase));
                ++it;
            }

            else if (timeSinceWaveArrival >= m_WaveDuration) {
                it = m_ActiveWaves.erase(it);
            }

            else {
                ++it;
            }
        }

        if (!waveColors.empty()) {
            renderer->setColor(blendWaveColors(waveColors));
        }

        else {
            renderer->setColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
        }
    }

    void Brick::render() {
        if (m_IsDestroyed) return;
        auto* renderer = getComponent<BoxRendererComponent>();

        if (renderer) {
            renderer->render();
        }
    }

    void Brick::setDestroyed(bool destroyed) {
        m_IsDestroyed = destroyed;
        auto* brickCollision = getComponent<CollisionComponent>();

        if (brickCollision) {
            brickCollision->setEnabled(!destroyed);
        }
    }

    void Brick::addWaveFromOrigin(Vector2 origin) {
        if (m_IsDestroyed) return;

        Vector2 brickCenter = m_Position + Vector2{ 25.0f, 15.0f };
        Vector2 diff = brickCenter - origin;

        float distance = std::sqrt(diff.getRawX() * diff.getRawX() + diff.getRawY() * diff.getRawY());

        WaveData wave;
        wave.distanceFromOrigin = distance;
        wave.startTime = m_GlobalTime;

        m_ActiveWaves.push_back(wave);
    }

    Color Brick::calculateWaveColor(float wavePhase) {
        float hue = wavePhase * 6.0f;
        float saturation = 1.0f;
        float value = 1.0f;

        float c = value * saturation;
        float x = c * (1.0f - std::abs(std::fmod(hue, 2.0f) - 1.0f));
        float m = value - c;

        float r, g, b = 0;
        int hi = ((int)hue) % 6;

        switch (hi) {
            case 0: r = c; g = x; b = 0; break;
            case 1: r = x; g = c; b = 0; break;
            case 2: r = 0; g = c; b = x; break;
            case 3: r = 0; g = x; b = c; break;
            case 4: r = x; g = 0; b = c; break;

            default: r = c; g = 0; b = x; break;
        }

        return Color(r + m, g + m, b + m, 1.0f);
    }

    Color Brick::blendWaveColors(const std::vector<Color>& colors) {
        if (colors.empty()) return Color(0.0f, 0.0f, 0.0f, 1.0f);
        if (colors.size() == 1) return colors[0];

        // Blend multiple colors by averaging their components
        float totalR = 0.0f, totalG = 0.0f, totalB = 0.0f;
        
        for (const Color& color : colors) {
            totalR += color.r;
            totalG += color.g;
            totalB += color.b;
        }

        float count = static_cast<float>(colors.size());
        return Color(totalR / count, totalG / count, totalB / count, 1.0f);
    }
}
