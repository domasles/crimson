#pragma once

namespace crimson {
    class Game {
        public:
            static Game& getInstance() {
                static Game instance;
                return instance;
            }

            bool init(const std::string& title, const int width, const int height, const bool vSync=true, const int targetFPS=60);
            void run();

            std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> m_Renderer;

        private:
            Game() : m_Window(nullptr, SDL_DestroyWindow), m_Renderer(nullptr, SDL_DestroyRenderer), m_Running(false), m_TargetFPS(0), m_FrameDelay(0), m_FrameTime(0), m_FrameStart(0) {}
            ~Game() { SDL_Quit(); }

            Game(const Game&) = delete;
            Game& operator=(const Game&) = delete;

            void processEvents();

            int m_TargetFPS;
            int m_FrameDelay;
            int m_FrameTime;

            bool m_Running;

            Uint32 m_FrameStart;

            std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> m_Window;
    };
}
