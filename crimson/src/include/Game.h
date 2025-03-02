#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <Logic.h>

#include <memory>
#include <string>

namespace crimson {
    class Game {
        public:
            static Game& getInstance() {
                static Game instance;
                return instance;
            }

            bool init(const std::string& title, const int width, const int height);
            void run();

            std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> m_Renderer;

        private:
            Game() : m_Window(nullptr, SDL_DestroyWindow), m_Renderer(nullptr, SDL_DestroyRenderer), m_Running(false) {}
            ~Game() { SDL_Quit(); }

            Game(const Game&) = delete;
            Game& operator=(const Game&) = delete;

            void processEvents();

            bool m_Running;

            std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> m_Window;
    };
}
