#pragma once

#include <utils/math.h>

#include <Texture.h>

using namespace engine::utils::math;

namespace engine::utils::rendering {
    enum QueueDirection
    {
        Normal,
        Reversed
    };

    class TileRenderQueue {
        public:
            struct TileRenderQueueItem {
                std::shared_ptr<Texture> texture = nullptr;

                Vector2 size{ 0.0f, 0.0f };
                Vector2 position{ 0.0f, 0.0f };
                Vector2 cropSize{ 0.0f, 0.0f };
                Vector2 cropPosition{ 0.0f, 0.0f };
            };

            void add(const std::shared_ptr<Texture>& texture, const Vector2& size, const Vector2& position);
            void add(const std::shared_ptr<Texture>& texture, const Vector2& size, const Vector2& position, const Vector2& cropSize, const Vector2& cropPosition);

            void setDirection(QueueDirection direction) { m_Direction = direction; }

            const std::deque<TileRenderQueueItem>& getItems() const { return m_Items; }

        private:
            QueueDirection m_Direction = Normal;
            std::deque<TileRenderQueueItem> m_Items;
    };

    class RenderQueueManager {
        public:
            static RenderQueueManager& getInstance();
            void render(const std::shared_ptr<TileRenderQueue>& queue);

        private:
            RenderQueueManager() = default;
            ~RenderQueueManager() = default;

            RenderQueueManager(const RenderQueueManager&) = delete;
            RenderQueueManager& operator=(const RenderQueueManager&) = delete;
    };
}
