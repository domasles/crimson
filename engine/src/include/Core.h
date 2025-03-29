#pragma once

#ifdef GAME_PLATFORM_WINDOWS
    #define GAME_API extern "C" __declspec(dllexport)
#elif GAME_PLATFORM_LINUX
    #define GAME_API extern "C" __attribute__((visibility("default")))
#endif

#ifdef ENGINE_PLATFORM_WINDOWS
    #define ENGINE_API extern "C" __declspec(dllexport)
#elif ENGINE_PLATFORM_LINUX
    #define ENGINE_API extern "C" __attribute__((visibility("default")))
#endif

#include <Input.h>

namespace engine {
    class Core {
        public:
            static Core& getInstance();

            const bool init(const std::string& parentFolder, const std::string& title, const int width=800, const int height=600, const bool fullScreen=false);
            const bool processEvents();

            void run(std::function<void()> customUpdate);

            SDL_Renderer* getRenderer() const;
            InputSystem* getInputSystem() const;

            const std::string& getName() const { return m_ParentFolder; }

        private:
            Core() : m_Window(nullptr, SDL_DestroyWindow), m_Renderer(nullptr, SDL_DestroyRenderer) {}
            ~Core() { SDL_Quit(); }

            Core(const Core&) = delete;
            Core& operator=(const Core&) = delete;

            int m_FrameDelay = 0;
            int m_FrameTime = 0;
            int m_FrameStart = 0;

            std::string m_ParentFolder;

            std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> m_Window;
            std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> m_Renderer;
            std::unique_ptr<InputSystem> m_InputSystem = nullptr;

            std::queue<SDL_Event> m_EventQueue;
    };
}
