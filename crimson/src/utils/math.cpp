#include <pch.h>

#include <utils/math.h>

namespace crimson::utils::math {
    Vector2& Vector2::operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;

        return *this;
    }

    Vector2 Vector2::normalize() const {
        float length = SDL_sqrtf(x * x + y * y);
        return (length > 0) ? Vector2{ x / length, y / length } : Vector2{ 0, 0 };
    }
}
