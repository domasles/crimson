#pragma once

#include <utils/math.h>

using namespace engine::utils::math;

namespace engine { class CollisionComponent; }

namespace engine::collisions {
    class BVH {
        public:
            static constexpr float FAT_MARGIN = 4.0f;

            bool update(const std::vector<engine::CollisionComponent*>& entities);
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
            std::unordered_map<engine::CollisionComponent*, AABB> m_FatAABBs;

            int m_Root = -1;

            void rebuild(const std::vector<engine::CollisionComponent*>& entities);
            int buildNode(std::vector<engine::CollisionComponent*>& entities, int start, int end);
            void queryNode(int idx, const AABB& aabb, std::vector<engine::CollisionComponent*>& results) const;
    };
}
