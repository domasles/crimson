#include <Game.h>

namespace crimson {
    bool Game::init(const std::string& title, int width, int height) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed: %s", SDL_GetError());
            return false;
        }

        m_Window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_RESIZABLE);

        if (!m_Window) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow failed: %s", SDL_GetError());
            return false;
        }

        m_Renderer = SDL_CreateRenderer(m_Window, nullptr);

        if (!m_Renderer) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer failed: %s", SDL_GetError());
            return false;
        }

        m_Logic.init();
        m_Running = true;

        return true;
    }

    void Game::run() {
        while (m_Running) {
            processEvents();

            m_Logic.handleInput();

            render();
        }
    }

    void Game::quit() {
        if (m_Renderer) {
            SDL_DestroyRenderer(m_Renderer);
        }

        if (m_Window) {
            SDL_DestroyWindow(m_Window);
        }

        SDL_Quit();
    }

    void Game::processEvents() {
        SDL_Event event;

        SDL_PollEvent(&event);

        if (event.type == SDL_EVENT_QUIT) {
            m_Running = false;
            quit();
        }
    }

    void Game::render() {
        SDL_RenderClear(m_Renderer);
        SDL_RenderPresent(m_Renderer);
    }
}
