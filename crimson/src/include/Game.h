#pragma once

#include <SDL3/SDL.h>
#include <Logic.h>

#include <string>

namespace crimson {
    class Game {
        public:
            Game() : m_Window(nullptr), m_Renderer(nullptr), m_Running(false) {}
            ~Game() { quit(); }

            bool init(const std::string& title, int width, int height);
            void run();
            void quit();

        private:
            void processEvents();
            void render();

            bool m_Running;

            SDL_Window* m_Window;
            SDL_Renderer* m_Renderer;

            Logic m_Logic;
    };
}
