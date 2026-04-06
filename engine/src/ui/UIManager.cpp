#include <pch.h>

#include <ui/UISystemInterface.h>
#include <ui/UIRenderer.h>
#include <ui/UIManager.h>

#include <utils/logger.h>

using namespace engine::utils::logger;

namespace engine::ui {
    UIManager::UIManager()  = default;
    UIManager::~UIManager() = default;

    UIManager& UIManager::getInstance() {
        static UIManager instance;
        return instance;
    }

    bool UIManager::init(engine::GLRenderer* renderer, int width, int height) {
        if (m_Initialized) {
            Logger::engine_warn("UIManager::init() called more than once - ignoring.");
            return true;
        }

        m_Width  = width;
        m_Height = height;

        m_SystemInterface = std::make_unique<UISystemInterface>();
        m_RenderBackend   = std::make_unique<UIRenderBackend>();

        Rml::SetSystemInterface(m_SystemInterface.get());
        Rml::SetRenderInterface(m_RenderBackend.get());

        if (!Rml::Initialise()) {
            Logger::engine_error("RmlUI initialization failed.");
            return false;
        }

        m_RenderBackend->init(renderer, width, height);

        m_Initialized = true;
        ENGINE_LOG_INIT("UI Manager");

        return true;
    }

    void UIManager::shutdown() {
        if (!m_Initialized) return;

        Rml::Shutdown();

        m_RenderBackend->shutdown();
        m_RenderBackend.reset();
        m_SystemInterface.reset();

        m_Initialized = false;
        ENGINE_LOG_INIT("UIManager shut down.");
    }

    void UIManager::loadFont(const std::string& path, bool fallback) {
        Rml::LoadFontFace(path, fallback);
    }

    void UIManager::updateSize(int width, int height, float dpRatio) {
        m_Width = width;
        m_Height = height;
        m_DpRatio = dpRatio;

        if (m_RenderBackend) m_RenderBackend->updateSize(width, height);
    }

    void UIManager::beginUIPass() {
        if (m_RenderBackend) m_RenderBackend->beginUIPass();
    }

    void UIManager::endUIPass() {
        if (m_RenderBackend) m_RenderBackend->endUIPass();
    }
}