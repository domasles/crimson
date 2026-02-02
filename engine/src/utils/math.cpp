#include <pch.h>

#include <utils/math.h>

namespace engine::utils::math {
    Vector2& Vector2::operator+=(const Vector2& other) {
        m_X += other.m_X;
        m_Y += other.m_Y;
        return *this;
    }

    Vector2& Vector2::operator-=(const Vector2& other) {
        m_X -= other.m_X;
        m_Y -= other.m_Y;
        return *this;
    }

    Vector2& Vector2::operator*=(const Vector2& other) {
        m_X *= other.m_X;
        m_Y *= other.m_Y;
        return *this;
    }

    Vector2& Vector2::operator/=(const Vector2& other) {
        m_X /= other.m_X;
        m_Y /= other.m_Y;
        return *this;
    }

    Vector2& Vector2::operator*=(float scalar) {
        m_X *= scalar;
        m_Y *= scalar;
        return *this;
    }

    Vector2& Vector2::operator/=(float scalar) {
        m_X /= scalar;
        m_Y /= scalar;
        return *this;
    }

    Vector2 Vector2::normalize() const {
        float len = std::sqrt(m_X * m_X + m_Y * m_Y);

        if (len > 0) {
            return Vector2{ m_X / len, m_Y / len };
        }

        else {
            return Vector2{ 0.0f, 0.0f };
        }
    }

    int Random::getInt(int min, int max) {
        static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng);
    }

    float Random::getFloat(float min, float max) {
        static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng);
    }

    bool Random::chance(float probability) {
        static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        std::bernoulli_distribution dist(probability);
        return dist(rng);
    }

    float Geometry::lerpAngle(float from, float to, float alpha) {
        float diff = to - from;

        while (diff > 180.0f) diff -= 360.0f;
        while (diff < -180.0f) diff += 360.0f;

        return from + diff * alpha;
    }
}
