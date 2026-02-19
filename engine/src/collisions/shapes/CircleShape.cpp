#include <pch.h>

#include <collisions/shapes/CircleShape.h>

namespace engine::collisions::shapes {
    Vector2 CircleShape::getSupportPoint(const Vector2& position, const Vector2& size, const Vector2& direction) const {
        float cx = position.getRawX() + size.getRawX() * 0.5f;
        float cy = position.getRawY() + size.getRawY() * 0.5f;

        float rx = size.getRawX() * 0.5f;
        float ry = size.getRawY() * 0.5f;

        float dx = direction.getRawX();
        float dy = direction.getRawY();

        float lenSq = (rx * dx) * (rx * dx) + (ry * dy) * (ry * dy);
        if (lenSq < 1e-10f) return Vector2{cx, cy};

        float len = std::sqrt(lenSq);
        return Vector2{cx + rx * rx * dx / len, cy + ry * ry * dy / len};
    }

    AABB CircleShape::getBoundingBox(const Vector2& position, const Vector2& size) const {
        return AABB{ position, position + size };
    }

    void CircleShape::renderGizmo(GLRenderer* renderer, const Vector2& position, const Vector2& size, const Color& color, float rotation) const {
        const int segments = 12;
        constexpr float PI = 3.14159265359f;

        static const std::array<Vector2, 13> s_UnitCircle = [PI] {
            std::array<Vector2, 13> circle;

            for (int i = 0; i <= 12; ++i) {
                float angle = (2.0f * PI * i) / 12;
                circle[i] = Vector2{std::cos(angle), std::sin(angle)};
            }

            return circle;
        }();

        float centerX = position.getX() + size.getX() * 0.5f;
        float centerY = position.getY() + size.getY() * 0.5f;

        float radiusX = size.getX() * 0.5f;
        float radiusY = size.getY() * 0.5f;

        for (int i = 0; i < segments; ++i) {
            const Vector2& p1 = s_UnitCircle[i];
            const Vector2& p2 = s_UnitCircle[i + 1];

            float x1 = centerX + radiusX * p1.getRawX();
            float y1 = centerY + radiusY * p1.getRawY();
            float x2 = centerX + radiusX * p2.getRawX();
            float y2 = centerY + radiusY * p2.getRawY();

            renderer->drawLine(Vector2{x1, y1}, Vector2{x2, y2}, color);
        }
    }
}
