#pragma once

#ifdef GAME_PLATFORM_WINDOWS
    #define GAME_API extern "C" __declspec(dllexport)
#elif GAME_PLATFORM_LINUX
    #define GAME_API extern "C" __attribute__((visibility("default")))
#endif

#include <Input.h>

namespace engine {
    class Core {
        public:
            static Core& getInstance();

            bool init(const std::string& title, const int width=800, const int height=600, const bool fullscreen=false);

            bool processEvents();
            void run();
            
            SDL_Renderer* getRenderer() const;
            InputSystem* getInputSystem() const;

        private:
            Core() : m_Window(nullptr, SDL_DestroyWindow), m_Renderer(nullptr, SDL_DestroyRenderer) {}
            ~Core() { SDL_Quit(); }

            Core(const Core&) = delete;
            Core& operator=(const Core&) = delete;

            int m_FrameDelay = 0;
            int m_FrameTime = 0;
            int m_FrameStart = 0;
            int m_FullscreenFlag = 0;

            std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> m_Window;
            std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> m_Renderer;
            std::unique_ptr<InputSystem> m_InputSystem = nullptr;

            std::queue<SDL_Event> m_EventQueue;
    };
}
