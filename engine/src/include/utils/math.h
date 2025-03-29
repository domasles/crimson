#pragma once

namespace engine::utils::math {
    struct Vector2 {
        Vector2& operator+=(const Vector2& other);
        Vector2& operator-=(const Vector2& other);

        Vector2& operator*=(const Vector2& other);
        Vector2& operator/=(const Vector2& other);

        Vector2 operator+(const Vector2& other) const { return { x + other.x, y + other.y }; }
        Vector2 operator-(const Vector2& other) const { return { x - other.x, y - other.y }; }

        Vector2 operator*(float scalar) const { return { x * scalar, y * scalar }; }
        Vector2 operator/(float scalar) const { return { x / scalar, y / scalar }; }

        Vector2 normalize() const;

        float x, y;
    };
}
