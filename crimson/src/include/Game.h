#pragma once

#include <SDL3/SDL.h>
#include <string>

namespace crimson {
    class Game {
        public:
            Game();
            ~Game();

            bool init(const std::string& title, int width, int height);
            void run();
            void quit();

        private:
            void update();
            void render();

            void handleInput();
            void processEvents();

            SDL_Window* m_Window;
            SDL_Renderer* m_Renderer;

            bool m_Running;
    };
}
