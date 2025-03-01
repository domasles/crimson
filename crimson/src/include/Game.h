#pragma once

#include <SDL3/SDL.h>
#include <Logic.h>
#include <memory>

#include <string>

namespace crimson {
    class Game {
        public:
            Game() : m_Window(nullptr, SDL_DestroyWindow), m_Renderer(nullptr, SDL_DestroyRenderer), m_Running(false) {}

            bool init(const std::string& title, int width, int height);
            void run();

        private:
            void processEvents();
            void render();

            bool m_Running;

            std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> m_Window;
            std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> m_Renderer;
    };
}
