#include <Game.h>

namespace crimson {
    bool Game::init(const std::string& title, const int width, const int height) {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed: %s", SDL_GetError());
            return false;
        }

        m_Window = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>(SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_RESIZABLE), SDL_DestroyWindow);

        if (!m_Window) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow failed: %s", SDL_GetError());
            return false;
        }

        m_Renderer = std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)>(SDL_CreateRenderer(m_Window.get(), nullptr), SDL_DestroyRenderer);

        if (!m_Renderer) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer failed: %s", SDL_GetError());
            return false;
        }

        Logic::getInstance().init();

        m_Running = true;

        return true;
    }

    void Game::run() {
        while (m_Running) {
            processEvents();

            Logic::getInstance().update();

            SDL_RenderClear(m_Renderer.get());
            Logic::getInstance().render();
            SDL_RenderPresent(m_Renderer.get());
        }
    }

    void Game::processEvents() {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                m_Running = false;
            }
        }
    }
}
