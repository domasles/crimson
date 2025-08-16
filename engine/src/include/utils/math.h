#pragma once

namespace engine::utils::math {
    class Vector2 {
        public:
            Vector2(float x = 0.0f, float y = 0.0f);
            ~Vector2();

            Vector2& operator+=(const Vector2& other);
            Vector2& operator-=(const Vector2& other);
            Vector2& operator*=(const Vector2& other);
            Vector2& operator/=(const Vector2& other);

            Vector2& operator*=(float scalar);
            Vector2& operator/=(float scalar);

            Vector2 operator+(const Vector2& other) const { return Vector2{ m_X + other.m_X, m_Y + other.m_Y }; }
            Vector2 operator-(const Vector2& other) const { return Vector2{ m_X - other.m_X, m_Y - other.m_Y }; }
            Vector2 operator*(const Vector2& other) const { return Vector2{ m_X * other.m_X, m_Y * other.m_Y }; }
            Vector2 operator/(const Vector2& other) const { return Vector2{ m_X / other.m_X, m_Y / other.m_Y }; }

            Vector2 operator*(float scalar) const { return Vector2{ m_X * scalar, m_Y * scalar }; }
            Vector2 operator/(float scalar) const { return Vector2{ m_X / scalar, m_Y / scalar }; }

            bool operator==(const Vector2& other) const { return m_X == other.m_X && m_Y == other.m_Y; }

            float getX() const { return m_ScaledX; }
            float getY() const { return m_ScaledY; }

            float getRawX() const { return m_X; }
            float getRawY() const { return m_Y; }

            void set(float x, float y);
            void update();

            static void setGlobalScale(float scaleX, float scaleY);
            static void updateAll();

            Vector2 getRawVector() const { return Vector2{ getRawX(), getRawY() }; }

            Vector2 normalize() const;

        private:
            float m_X = 0.0f;
            float m_Y = 0.0f;

            float m_ScaledX = 0.0f;
            float m_ScaledY = 0.0f;

            inline static std::vector<Vector2*> s_Instances;

            inline static float s_GlobalScaleX = 0.0f;
            inline static float s_GlobalScaleY = 0.0f;
    };
}
