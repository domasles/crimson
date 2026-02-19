#include <pch.h>

#include <components/CollisionComponent.h>
#include <collisions/BVH.h>

namespace engine::collisions {

    void BVH::clear() {
        m_Nodes.clear();
        m_FatAABBs.clear();

        m_Root = -1;
    }

    void BVH::rebuild(const std::vector<engine::CollisionComponent*>& entities) {
        clear();

        if (entities.empty()) return;

        m_Nodes.reserve(2 * entities.size());
        m_FatAABBs.reserve(entities.size());

        std::vector<engine::CollisionComponent*> sorted = entities;
        m_Root = buildNode(sorted, 0, (int)sorted.size());
    }

    bool BVH::update(const std::vector<engine::CollisionComponent*>& entities) {
        if (m_Root == -1 || entities.size() != m_FatAABBs.size()) {
            rebuild(entities);
            return true;
        }

        for (auto* comp : entities) {
            Vector2 pos = comp->getCollisionWorldPosition();
            Vector2 size = comp->getCollision().size;

            AABB tight = comp->getCollision().shape->getBoundingBox(pos, size);
            auto it = m_FatAABBs.find(comp);

            if (it == m_FatAABBs.end() || !it->second.contains(tight)) {
                rebuild(entities);
                return true;
            }
        }

        return false;
    }

    int BVH::buildNode(std::vector<engine::CollisionComponent*>& entities, int start, int end) {
        if (end - start == 1) {
            auto* comp = entities[start];

            Vector2 pos = comp->getCollisionWorldPosition();
            Vector2 size = comp->getCollision().size;

            Node node;
            node.aabb = comp->getCollision().shape->getBoundingBox(pos, size);
            node.entity = comp;

            AABB fat{
                Vector2{node.aabb.min.getRawX() - FAT_MARGIN, node.aabb.min.getRawY() - FAT_MARGIN},
                Vector2{node.aabb.max.getRawX() + FAT_MARGIN, node.aabb.max.getRawY() + FAT_MARGIN}
            };

            m_FatAABBs[comp] = fat;

            int idx = (int)m_Nodes.size();
            m_Nodes.push_back(node);
            return idx;
        }

        AABB combined = m_Nodes.empty() ? AABB{} : AABB{};
        bool first = true;

        for (int i = start; i < end; i++) {
            auto* comp = entities[i];

            Vector2 pos = comp->getCollisionWorldPosition();
            Vector2 size = comp->getCollision().size;

            AABB box = comp->getCollision().shape->getBoundingBox(pos, size);

            if (first) { combined = box; first = false; }
            else combined = combined.merge(box);
        }

        float extX = combined.max.getRawX() - combined.min.getRawX();
        float extY = combined.max.getRawY() - combined.min.getRawY();

        int mid = (start + end) / 2;

        if (extX >= extY) {
            std::nth_element(entities.begin() + start, entities.begin() + mid, entities.begin() + end,
                [](const engine::CollisionComponent* a, const engine::CollisionComponent* b) {
                    float cA = a->getCollisionWorldPosition().getRawX() + a->getCollision().size.getRawX() * 0.5f;
                    float cB = b->getCollisionWorldPosition().getRawX() + b->getCollision().size.getRawX() * 0.5f;

                    return cA < cB;
                });
        }

        else {
            std::nth_element(entities.begin() + start, entities.begin() + mid, entities.begin() + end,
                [](const engine::CollisionComponent* a, const engine::CollisionComponent* b) {
                    float cA = a->getCollisionWorldPosition().getRawY() + a->getCollision().size.getRawY() * 0.5f;
                    float cB = b->getCollisionWorldPosition().getRawY() + b->getCollision().size.getRawY() * 0.5f;

                    return cA < cB;
                });
        }

        int left = buildNode(entities, start, mid);
        int right = buildNode(entities, mid, end);

        Node node;

        node.left = left;
        node.right = right;
        node.aabb = m_Nodes[left].aabb.merge(m_Nodes[right].aabb);

        int idx = (int)m_Nodes.size();
        m_Nodes.push_back(node);
        return idx;
    }

    void BVH::query(const AABB& aabb, std::vector<engine::CollisionComponent*>& results) const {
        if (m_Root == -1) return;
        queryNode(m_Root, aabb, results);
    }

    void BVH::queryNode(int idx, const AABB& aabb, std::vector<engine::CollisionComponent*>& results) const {
        const Node& node = m_Nodes[idx];

        if (!node.aabb.overlaps(aabb)) return;

        if (node.isLeaf()) {
            results.push_back(node.entity);
            return;
        }

        if (node.left != -1) queryNode(node.left, aabb, results);
        if (node.right != -1) queryNode(node.right, aabb, results);
    }
}
