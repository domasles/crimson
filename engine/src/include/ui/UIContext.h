#pragma once

#include <SDL3/SDL_events.h>

#include <ui/UIDocument.h>

namespace Rml {
    class Context;
}

namespace engine::ui {
    class UIContext;

    UIContext& getCurrentContext();

    class UIContext {
        public:
            UIContext() = default;
            ~UIContext();

            UIContext(const UIContext&) = delete;
            UIContext& operator=(const UIContext&) = delete;

            UIContext(UIContext&&) = delete;
            UIContext& operator=(UIContext&&) = delete;

            void init(int width, int height);
            void shutdown();

            bool initialized() const { return m_Context != nullptr; }

            void update();
            void render();

            bool processEvent(const SDL_Event& event);
            bool isCapturingInput() const;

            UIDocument loadDocument(const std::string& path);

        private:
            Rml::Context* m_Context = nullptr;
    };
}
