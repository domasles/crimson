#include <pch.h>

#include <Game.h>
#include <Logic.h>

namespace crimson {
    bool Game::init(const std::string& title, const int width, const int height, const bool fullscreen, const bool vSync, int targetFPS) {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed: %s", SDL_GetError());
            return false;
        }

        if (fullscreen) {
            m_FullscreenFlag = SDL_WINDOW_FULLSCREEN;
        }

        else {
            m_FullscreenFlag = SDL_WINDOW_RESIZABLE;
        }

        m_Window = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>(SDL_CreateWindow(title.c_str(), width, height, m_FullscreenFlag), SDL_DestroyWindow);

        if (!m_Window) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow failed: %s", SDL_GetError());
            return false;
        }

        m_Renderer = std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)>(SDL_CreateRenderer(m_Window.get(), nullptr), SDL_DestroyRenderer);

        if (!m_Renderer) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer failed: %s", SDL_GetError());
            return false;
        }

        if (!SDL_SetRenderVSync(m_Renderer.get(), vSync)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_SetRenderVSync failed: %s", SDL_GetError());
            return false;
        }

        Logic::getInstance().init();

        if (vSync) {
            targetFPS = -1;
        }
        
        m_FrameDelay = 1000 / targetFPS;
        m_Running = true;

        return true;
    }

    void Game::run() {
        auto lastTime = steady_clock::now();

        while (m_Running) {
            auto startTime = steady_clock::now();
            float deltaTime = duration<float>(startTime - lastTime).count();

            lastTime = startTime;

            processEvents();
            Logic::getInstance().update();

            SDL_RenderClear(m_Renderer.get());
            Logic::getInstance().render();
            SDL_RenderPresent(m_Renderer.get());

            auto frameTime = duration_cast<milliseconds>(steady_clock::now() - startTime).count();

            if (m_FrameDelay > frameTime) {
                std::this_thread::sleep_for(milliseconds(m_FrameDelay - frameTime));
            }
        }
    }

    void Game::processEvents() {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            m_EventQueue.push(event);
        }

        while (!m_EventQueue.empty()) {
            SDL_Event e = m_EventQueue.front();
            m_EventQueue.pop();

            if (e.type == SDL_EVENT_QUIT) {
                m_Running = false;
            }
        }
    }

    SDL_Renderer* Game::getRenderer() const {
        if (!m_Renderer) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer is not initialized yet!");
            return nullptr;
        }

        return m_Renderer.get();
    }
}
