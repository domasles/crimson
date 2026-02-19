#pragma once

#include <utils/math.h>

using namespace engine::utils::math;

namespace engine { class CollisionComponent; }

namespace engine::collisions {
    class BVH {
        public:
            void rebuild(const std::vector<engine::CollisionComponent*>& entities);
            void query(const AABB& aabb, std::vector<engine::CollisionComponent*>& results) const;
            void clear();

        private:
            struct Node {
                AABB aabb;
                engine::CollisionComponent* entity = nullptr;

                int left = -1;
                int right = -1;

                bool isLeaf() const { return left == -1; }
            };

            std::vector<Node> m_Nodes;

            int m_Root = -1;

            int buildNode(std::vector<engine::CollisionComponent*>& entities, int start, int end);
            void queryNode(int idx, const AABB& aabb, std::vector<engine::CollisionComponent*>& results) const;
    };
}
