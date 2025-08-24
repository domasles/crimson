#include <pch.h>

#include <entities/Brick.h>

namespace outBreak {
    // Constants for brick size (should match MainScene values)
    const float BRICK_WIDTH = 80.0f;
    const float BRICK_HEIGHT = 30.0f;

    void Brick::init() {
        auto* transform = addComponent<TransformComponent>();
        auto* renderer = addComponent<BoxRendererComponent>();

        transform->setSize({BRICK_WIDTH, BRICK_HEIGHT});
        renderer->setColor(Color(0.0f, 0.0f, 0.0f, 1.0f)); // Black initially
        
        // Store position for wave calculations
        m_Position = transform->getPosition();
    }

    void Brick::update(float deltaTime) {
        updateComponents(deltaTime);

        // Update global time for wave calculations
        m_GlobalTime += deltaTime;

        auto* renderer = getComponent<BoxRendererComponent>();
        if (!renderer) return;

        if (m_IsDestroyed) {
            // Don't update destroyed bricks
            return;
        }

        // Update all active waves and collect colors
        std::vector<Color> waveColors;
        
        // Remove expired waves and update active ones
        for (auto it = m_ActiveWaves.begin(); it != m_ActiveWaves.end();) {
            WaveData& wave = *it;
            
            // Calculate when the wave should reach this brick
            float waveArrivalTime = wave.startTime + (wave.distanceFromOrigin / m_WaveSpeed);
            float timeSinceWaveArrival = m_GlobalTime - waveArrivalTime;

            if (timeSinceWaveArrival >= 0.0f && timeSinceWaveArrival < m_WaveDuration) {
                // Wave is currently passing through this brick
                float wavePhase = timeSinceWaveArrival / m_WaveDuration;
                waveColors.push_back(calculateWaveColor(wavePhase));
                ++it;
            } else if (timeSinceWaveArrival >= m_WaveDuration) {
                // Wave has passed - remove it
                it = m_ActiveWaves.erase(it);
            } else {
                // Wave hasn't arrived yet
                ++it;
            }
        }

        // Set color based on active waves
        if (!waveColors.empty()) {
            // Blend multiple wave colors if overlapping
            renderer->setColor(blendWaveColors(waveColors));
        } else if (m_RainbowEffect) {
            // Handle old rainbow effect (fallback)
            m_RainbowTime += deltaTime;
            renderer->setColor(calculateRainbowColor(m_RainbowTime));

            if (m_RainbowTime >= m_RainbowDuration) {
                m_RainbowEffect = false;
                m_RainbowTime = 0.0f;
                renderer->setColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
            }
        } else {
            // Default to black
            renderer->setColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
        }
    }

    void Brick::render() {
        // Don't render destroyed bricks at all
        if (m_IsDestroyed) {
            return;
        }

        // Call render on the BoxRendererComponent
        auto* renderer = getComponent<BoxRendererComponent>();
        if (renderer) {
            renderer->render();
        }
    }

    void Brick::addWaveFromOrigin(Vector2 origin) {
        if (m_IsDestroyed) return; // Destroyed bricks don't participate in waves

        // Calculate distance from origin to this brick's center
        Vector2 brickCenter = m_Position + Vector2(25.0f, 15.0f); // Half brick size
        Vector2 diff = brickCenter - origin;
        float distance = std::sqrt(diff.getRawX() * diff.getRawX() + diff.getRawY() * diff.getRawY());

        WaveData wave;
        wave.distanceFromOrigin = distance;
        wave.startTime = m_GlobalTime;
        
        m_ActiveWaves.push_back(wave);
    }

    Color Brick::calculateWaveColor(float wavePhase) {
        // Create smooth rainbow transition during wave passage
        float hue = wavePhase * 6.0f; // Full rainbow cycle during passage
        float saturation = 1.0f;
        float value = 1.0f;

        // HSV to RGB conversion
        float c = value * saturation;
        float x = c * (1.0f - std::abs(std::fmod(hue, 2.0f) - 1.0f));
        float m = value - c;

        float r, g, b;
        
        if (hue >= 0 && hue < 1) {
            r = c; g = x; b = 0;
        } else if (hue >= 1 && hue < 2) {
            r = x; g = c; b = 0;
        } else if (hue >= 2 && hue < 3) {
            r = 0; g = c; b = x;
        } else if (hue >= 3 && hue < 4) {
            r = 0; g = x; b = c;
        } else if (hue >= 4 && hue < 5) {
            r = x; g = 0; b = c;
        } else {
            r = c; g = 0; b = x;
        }

        return Color(r + m, g + m, b + m, 1.0f);
    }

    Color Brick::blendWaveColors(const std::vector<Color>& colors) {
        if (colors.empty()) {
            return Color(0.0f, 0.0f, 0.0f, 1.0f); // Black default
        }
        
        if (colors.size() == 1) {
            return colors[0];
        }

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

    Color Brick::calculateRainbowColor(float time) {
        // Create rainbow effect using HSV to RGB conversion
        // Cycle through hues over time
        float hue = std::fmod(time * 2.0f, 6.0f); // Complete rainbow cycle every 3 seconds
        float saturation = 1.0f;
        float value = 1.0f;

        // HSV to RGB conversion
        float c = value * saturation;
        float x = c * (1.0f - std::abs(std::fmod(hue, 2.0f) - 1.0f));
        float m = value - c;

        float r, g, b;
        
        if (hue >= 0 && hue < 1) {
            r = c; g = x; b = 0;
        } else if (hue >= 1 && hue < 2) {
            r = x; g = c; b = 0;
        } else if (hue >= 2 && hue < 3) {
            r = 0; g = c; b = x;
        } else if (hue >= 3 && hue < 4) {
            r = 0; g = x; b = c;
        } else if (hue >= 4 && hue < 5) {
            r = x; g = 0; b = c;
        } else {
            r = c; g = 0; b = x;
        }

        return Color(r + m, g + m, b + m, 1.0f);
    }
}
