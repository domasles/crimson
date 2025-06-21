#include <pch.h>

#include <utils/rendering.h>
#include <utils/logger.h>

using namespace engine::utils::logger;

namespace engine::utils::rendering {
    void TileRenderQueue::add(const std::shared_ptr<Texture>& texture, const Vector2& size, const Vector2& position) {
        TileRenderQueueItem queue{ texture, size, position, {}, {} };

        if (m_Direction == Normal) {
            m_Items.push_back(queue);
        }

        else {
            m_Items.push_front(queue);
        }
    }

    void TileRenderQueue::add(const std::shared_ptr<Texture>& texture, const Vector2& size, const Vector2& position, const Vector2& cropSize, const Vector2& cropPosition) {
        TileRenderQueueItem queue{ texture, size, position, cropSize, cropPosition };

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
            Logger::engine_error("Memory allocation failed: %s", e.what());
        }

        static RenderQueueManager fallbackInstance;
        return fallbackInstance;
    }

    void RenderQueueManager::render(const std::shared_ptr<TileRenderQueue>& queue) {
        for (const auto& item : queue->getItems()) {
            Vector2 size = item.size.getRawVector();
            Vector2 position = item.position.getRawVector();

            item.texture->render(size, position, item.cropSize, item.cropPosition);
        }
    }
}
