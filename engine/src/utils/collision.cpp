#include <utils/collision.h>
#include <unordered_map>

namespace engine::utils::collision {

    namespace dispatch {
        // Collision function registry - automatically handles any shape combination
        static std::unordered_map<int, CollisionFunction> s_CollisionRegistry;
        
        // Create unique key for shape pair
        inline int makeShapeKey(CollisionShape shape1, CollisionShape shape2) {
            return (static_cast<int>(shape1) << 16) | static_cast<int>(shape2);
        }
        
        void registerShapeCollision(CollisionShape shape1, CollisionShape shape2, CollisionFunction func) {
            s_CollisionRegistry[makeShapeKey(shape1, shape2)] = func;
            // Auto-register symmetric case if different
            if (shape1 != shape2) {
                s_CollisionRegistry[makeShapeKey(shape2, shape1)] = func;
            }
        }
        
        CollisionFunction getCollisionFunction(CollisionShape shape1, CollisionShape shape2) {
            auto it = s_CollisionRegistry.find(makeShapeKey(shape1, shape2));
            return (it != s_CollisionRegistry.end()) ? it->second : nullptr;
        }
        
        bool checkShapeCollision(CollisionShape shape1, const Vector2& pos1, const Vector2& size1,
                                CollisionShape shape2, const Vector2& pos2, const Vector2& size2) {
            
            // Get appropriate collision function automatically
            CollisionFunction func = getCollisionFunction(shape1, shape2);
            if (func) {
                return func(pos1, size1, pos2, size2);
            }
            
            return false; // No collision function registered for this shape combination
        }
        
        // Auto-initialization - register default shapes
        static bool initializeDefaultShapes() {
            // Register BOX vs BOX collision
            registerShapeCollision(CollisionShape::BOX, CollisionShape::BOX, BoxShape::checkCollision);
            
            // Future shapes auto-register themselves:
            // registerShapeCollision(CollisionShape::CIRCLE, CollisionShape::CIRCLE, CircleShape::checkCollision);
            // registerShapeCollision(CollisionShape::BOX, CollisionShape::CIRCLE, BoxCircleCollision::check);
            
            return true;
        }
        
        static bool s_Initialized = initializeDefaultShapes();
    }

    bool checkCollision(const Collision& entityCollision, const Vector2& entityPos,
                       const CollisionTile& tileCollision) {
        
        // Skip collision if either has NONE type
        if (entityCollision.type == CollisionType::NONE || tileCollision.type == CollisionType::NONE) {
            return false;
        }
        
        // Calculate entity collision world position
        Vector2 entityCollisionPos = {
            entityPos.getRawX() + entityCollision.offset.getRawX(),
            entityPos.getRawY() + entityCollision.offset.getRawY()
        };
        
        // Automatic collision dispatch - NO manual if-statements!
        return dispatch::checkShapeCollision(entityCollision.shape, entityCollisionPos, entityCollision.size,
                                           tileCollision.shape, tileCollision.worldPosition, tileCollision.size);
    }
    
    bool checkCollision(const Collision& collision1, const Vector2& pos1,
                       const Collision& collision2, const Vector2& pos2) {
        
        // Skip collision if either has NONE type
        if (collision1.type == CollisionType::NONE || collision2.type == CollisionType::NONE) {
            return false;
        }
        
        // Calculate collision world positions
        Vector2 collisionPos1 = {
            pos1.getRawX() + collision1.offset.getRawX(),
            pos1.getRawY() + collision1.offset.getRawY()
        };
        
        Vector2 collisionPos2 = {
            pos2.getRawX() + collision2.offset.getRawX(),
            pos2.getRawY() + collision2.offset.getRawY()
        };
        
        // Automatic collision dispatch - NO manual if-statements!
        return dispatch::checkShapeCollision(collision1.shape, collisionPos1, collision1.size,
                                           collision2.shape, collisionPos2, collision2.size);
    }

}
