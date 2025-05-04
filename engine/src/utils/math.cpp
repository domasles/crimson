#include <pch.h>

#include <utils/math.h>

namespace engine::utils::math {
    Vector2::Vector2(float x, float y) : m_X(x), m_Y(y), m_ScaledX(x), m_ScaledY(y) {
        s_Instances.push_back(this);
        update();
    }

    Vector2::~Vector2() {
        auto it = std::find(s_Instances.begin(), s_Instances.end(), this);

        if (it != s_Instances.end()) {
            s_Instances.erase(it);
        }
    }

    Vector2& Vector2::operator+=(const Vector2& other) {
        m_X += other.m_X;
        m_Y += other.m_Y;

        update();

        return *this;
    }

    Vector2& Vector2::operator-=(const Vector2& other) {
        m_X -= other.m_X;
        m_Y -= other.m_Y;

        update();

        return *this;
    }

    Vector2& Vector2::operator*=(const Vector2& other) {
        m_X *= other.m_X;
        m_Y *= other.m_Y;

        update();

        return *this;
    }

    Vector2& Vector2::operator/=(const Vector2& other) {
        m_X /= other.m_X;
        m_Y /= other.m_Y;

        update();

        return *this;
    }

    Vector2& Vector2::operator*=(float scalar) {
        m_X *= scalar;
        m_Y *= scalar;

        update();

        return *this;
    }

    Vector2& Vector2::operator/=(float scalar) {
        m_X /= scalar;
        m_Y /= scalar;

        update();

        return *this;
    }

    void Vector2::set(float x, float y) {
        m_X = x;
        m_Y = y;

        update();
    }

    void Vector2::update() {
        m_ScaledX = m_X * s_GlobalScaleX;
        m_ScaledY = m_Y * s_GlobalScaleY;
    }

    void Vector2::setGlobalScale(float scaleX, float scaleY) {
        s_GlobalScaleX = scaleX;
        s_GlobalScaleY = scaleY;
    }

    void Vector2::updateAll() {
        for (auto* v : s_Instances) {
            v->update();
        }
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
}
