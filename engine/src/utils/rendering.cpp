#include <pch.h>

#include <utils/rendering.h>

namespace engine::utils::rendering {
    void TileRenderQueue::add(const std::shared_ptr<Texture>& texture, const Vector2& size, const Vector2& position) {
        std::array<float, 2> rawSize{ size.getRawX(), size.getRawY() };
        std::array<float, 2> rawPosition{ position.getRawX(), position.getRawY() };

        TileRenderQueueItem queue{ texture, rawSize, rawPosition, {}, {} };

        if (m_Direction == Normal) {
            m_Items.push_back(queue);
        }

        else {
            m_Items.push_front(queue);
        }
    }

    void TileRenderQueue::add(const std::shared_ptr<Texture>& texture, const Vector2& size, const Vector2& position, const Vector2& cropSize, const Vector2& cropPosition) {
        std::array<float, 2> rawSize{ size.getRawX(), size.getRawY() };
        std::array<float, 2> rawPosition{ position.getRawX(), position.getRawY() };

        TileRenderQueueItem queue{ texture, rawSize, rawPosition, cropSize, cropPosition };

        if (m_Direction == Normal) {
            m_Items.push_back(queue);
        }

        else {
            m_Items.push_front(queue);
        }
    }

    RenderQueueManager& RenderQueueManager::getInstance() {
        try {
            static RenderQueueManager& instance = *new RenderQueueManager();
            return instance;
        }

        catch (const std::bad_alloc& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Memory allocation failed: %s", e.what());
        }

        static RenderQueueManager fallbackInstance;
        return fallbackInstance;
    }

    void RenderQueueManager::render(const std::shared_ptr<TileRenderQueue>& queue) {
        for (const auto& item : queue->getItems()) {
            Vector2 size{ item.size[0], item.size[1] };
            Vector2 position{ item.position[0], item.position[1] };

            item.texture->render(size, position, item.cropSize, item.cropPosition);
        }
    }
}