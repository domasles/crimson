#pragma once

using namespace std::chrono;

namespace crimson {
    class Game {
        public:
            static Game& getInstance();

            bool init(const std::string& title, const int width=800, const int height=600, const bool fullscreen=false, const bool vSync=true, int targetFPS=60);
            void run();

            SDL_Renderer* getRenderer() const;

        private:
            Game() : m_Window(nullptr, SDL_DestroyWindow), m_Renderer(nullptr, SDL_DestroyRenderer) {}
            ~Game() { SDL_Quit(); }

            Game(const Game&) = delete;
            Game& operator=(const Game&) = delete;

            void processEvents();

            int m_FrameDelay = 0;
            int m_FrameTime = 0;
            int m_FrameStart = 0;
            int m_FullscreenFlag = 0;

            bool m_Running = false;

            std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> m_Window;
            std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> m_Renderer;

            std::queue<SDL_Event> m_EventQueue;
    };
}
