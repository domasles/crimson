#pragma once

#include <string>

#include <ui/UISystemInterface.h>
#include <ui/UIRenderBackend.h>

namespace Rml {
    class Context;
    class ElementDocument;
}

namespace engine {
    class GLRenderer;
}

namespace engine::ui {
    class UIManager {
        public:
            static UIManager& getInstance();

            bool init(engine::GLRenderer* renderer, int width, int height);
            void shutdown();

            void update();
            void render();

            bool processEvent(const SDL_Event& event);
            bool isCapturingInput() const;

            void unloadDocument(Rml::ElementDocument* doc);

            Rml::ElementDocument* loadDocument(const std::string& path);

        private:
            UIManager()  = default;
            ~UIManager() = default;

            UIManager(const UIManager&)            = delete;
            UIManager& operator=(const UIManager&) = delete;

            int getRmlModifiers() const;

            Rml::Input::KeyIdentifier toRmlKey(SDL_Keycode key) const;

            int  m_Width = 0;
            int  m_Height = 0;
            bool m_Initialized = false;

            UISystemInterface m_SystemInterface;
            UIRenderBackend   m_RenderBackend;

            Rml::Context* m_Context = nullptr;
    };

    inline UIManager& getUI() { return UIManager::getInstance(); }
}
