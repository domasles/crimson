#pragma once

namespace engine::utils::math {
    class Vector2 {
        public:
            constexpr Vector2(float x = 0.0f, float y = 0.0f) : m_X(x), m_Y(y) {}
            ~Vector2() = default;

            Vector2& operator+=(const Vector2& other);
            Vector2& operator-=(const Vector2& other);
            Vector2& operator*=(const Vector2& other);
            Vector2& operator/=(const Vector2& other);

            Vector2& operator*=(float scalar);
            Vector2& operator/=(float scalar);

            constexpr Vector2 operator+(const Vector2& other) const { return Vector2{ m_X + other.m_X, m_Y + other.m_Y }; }
            constexpr Vector2 operator-(const Vector2& other) const { return Vector2{ m_X - other.m_X, m_Y - other.m_Y }; }
            constexpr Vector2 operator*(const Vector2& other) const { return Vector2{ m_X * other.m_X, m_Y * other.m_Y }; }
            constexpr Vector2 operator/(const Vector2& other) const { return Vector2{ m_X / other.m_X, m_Y / other.m_Y }; }

            constexpr Vector2 operator*(float scalar) const { return Vector2{ m_X * scalar, m_Y * scalar }; }
            constexpr Vector2 operator/(float scalar) const { return Vector2{ m_X / scalar, m_Y / scalar }; }

            constexpr bool operator==(const Vector2& other) const { return m_X == other.m_X && m_Y == other.m_Y; }

            constexpr float getX() const { return m_X; }
            constexpr float getY() const { return m_Y; }

            constexpr float getRawX() const { return m_X; }
            constexpr float getRawY() const { return m_Y; }

            void set(float x, float y) { m_X = x; m_Y = y; }

            Vector2 normalize() const;

        private:
            float m_X = 0.0f;
            float m_Y = 0.0f;
    };

    class Random {
        public:
            static int getInt(int min, int max);
            static float getFloat(float min, float max);
            static bool chance(float probability);
    };
}
