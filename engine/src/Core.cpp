#include <pch.h>

#include <utils/math.h>

#include <Scene.h>
#include <Core.h>

ENGINE_API void internalUpdate(std::function<void()> customUpdate) {
    engine::Core::getInstance().run(customUpdate);
}

namespace engine {
    Core& Core::getInstance() {
        try {
            static Core& instance = *new Core();
            return instance;
        }

        catch (const std::bad_alloc& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Memory allocation failed: %s", e.what());
        }

        static Core fallbackInstance;
        return fallbackInstance;
    }

    const bool Core::init(const std::string& parentFolder, const std::string& title, const int width, const int height, const bool fullScreen) {
        m_ParentFolder = parentFolder;

        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed: %s", SDL_GetError());
            return false;
        }

        m_Window = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>(SDL_CreateWindow(title.c_str(), width, height, (int)fullScreen | SDL_WINDOW_RESIZABLE), SDL_DestroyWindow);

        if (!m_Window) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow failed: %s", SDL_GetError());
            return false;
        }

        m_Renderer = std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)>(SDL_CreateRenderer(m_Window.get(), nullptr), SDL_DestroyRenderer);

        if (!m_Renderer) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer failed: %s", SDL_GetError());
            return false;
        }

        if (!SDL_SetRenderVSync(m_Renderer.get(), true)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_SetRenderVSync failed: %s", SDL_GetError());
            return false;
        }

        m_InputSystem = std::make_unique<InputSystem>();
        return true;
    }

    void Core::run(std::function<void()> customUpdate) {
        while (processEvents()) {
            SDL_RenderClear(m_Renderer.get());

            if (customUpdate) {
                customUpdate();
            }

            SceneManager::getInstance().update();
            SceneManager::getInstance().render();

            SDL_RenderPresent(m_Renderer.get());
        }
    }

    const bool Core::processEvents() {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            m_EventQueue.push(event);
        }

        while (!m_EventQueue.empty()) {
            SDL_Event e = m_EventQueue.front();
            m_EventQueue.pop();

            if (e.type == SDL_EVENT_QUIT) {
                return false;
            }
        }

        return true;
    }

    SDL_Renderer* Core::getRenderer() const {
        if (!m_Renderer) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer is not initialized yet!");
            return nullptr;
        }

        return m_Renderer.get();
    }

    InputSystem* Core::getInputSystem() const {
        if (!m_InputSystem) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Input system is not initialized yet!");
            return nullptr;
        }

        return m_InputSystem.get();
    }
}
