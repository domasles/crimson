#pragma once

namespace engine {
    class GLRenderer;
}

namespace engine::ui {
    class UISystemInterface;
    class UIRenderBackend;

    class UIManager {
        public:
            static UIManager& getInstance();

            UIManager();
            ~UIManager();

            bool init(engine::GLRenderer* renderer, int width, int height);
            void shutdown();

            void loadFont(const std::string& path, bool fallback = false);

            void beginUIPass();
            void endUIPass();

            int getWidth()  const { return m_Width; }
            int getHeight() const { return m_Height; }

        private:
            UIManager(const UIManager&)            = delete;
            UIManager& operator=(const UIManager&) = delete;

            int  m_Width = 0;
            int  m_Height = 0;
            bool m_Initialized = false;

            std::unique_ptr<UISystemInterface> m_SystemInterface;
            std::unique_ptr<UIRenderBackend> m_RenderBackend;
    };
}
