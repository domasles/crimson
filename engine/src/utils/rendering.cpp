#include <pch.h>

#include <utils/singleton.h>
#include <utils/rendering.h>
#include <utils/logger.h>

using namespace engine::utils::logger;
using namespace engine::utils;

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
        return singleton<RenderQueueManager>();
    }
    
    void RenderQueueManager::render(const std::shared_ptr<TileRenderQueue>& queue) {
        for (const auto& [texture, size, position, cropSize, cropPosition] : queue->getItems()) {
            texture->render(size, position, cropSize, cropPosition);
        }
    }

    Color::Color(float r, float g, float b, float a)
        : r(std::clamp(r, 0.0f, 1.0f)),
          g(std::clamp(g, 0.0f, 1.0f)),
          b(std::clamp(b, 0.0f, 1.0f)),
          a(std::clamp(a, 0.0f, 1.0f))
    {}
}
