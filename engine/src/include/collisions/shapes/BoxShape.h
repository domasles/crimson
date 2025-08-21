#pragma once
#include <utils/math.h>

using namespace engine::utils::math;

namespace engine::collisions::shapes {
    struct BoxShape {
        static bool checkCollision(const Vector2& pos1, const Vector2& size1,
                                 const Vector2& pos2, const Vector2& size2) {
            return (pos1.getRawX() < pos2.getRawX() + size2.getRawX() &&
                    pos1.getRawX() + size1.getRawX() > pos2.getRawX() &&
                    pos1.getRawY() < pos2.getRawY() + size2.getRawY() &&
                    pos1.getRawY() + size1.getRawY() > pos2.getRawY());
        }
        
        static bool pointInside(const Vector2& point, const Vector2& pos, const Vector2& size) {
            return (point.getRawX() >= pos.getRawX() && 
                    point.getRawX() <= pos.getRawX() + size.getRawX() &&
                    point.getRawY() >= pos.getRawY() && 
                    point.getRawY() <= pos.getRawY() + size.getRawY());
        }
    };
}
