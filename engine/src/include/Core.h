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

            const bool init(const std::string& workingDir, const std::string& title, const int width=800, const int height=600, const bool resizable=false);
            const bool init(const std::string& workingDir, const std::string& title, const bool fullScreen=false);

            const bool processEvents();
            const std::string& getName() const { return m_ParentFolder; }

            void run(std::function<void()> customUpdate);

            void setVectorScale(int targetWindowWidth, int targetWindowHeight);
            void setVectorScale(bool useDefaultScale=true);

            SDL_Renderer* getRenderer() const;
            SDL_Window* getWindow() const;

            Vector2 getWindowSize() const { return Vector2{ static_cast<float>(m_WindowWidth), static_cast<float>(m_WindowHeight) }; }

        private:
            Core() : m_Window(nullptr, SDL_DestroyWindow), m_Renderer(nullptr, SDL_DestroyRenderer) {}
            ~Core() { SDL_Quit(); }

            Core(const Core&) = delete;
            Core& operator=(const Core&) = delete;

            void initWindowedWindow(const std::string& title, const int width, const int height, const bool resizable);
            void initFullScreenWindow(const std::string& title);

            void initRenderer();
            void updateVectorScale();

            float calculateUniformScale(int width, int height, float baseWidth, float baseHeight);

            int m_WindowWidth = 0;
            int m_WindowHeight = 0;

            int m_TargetWindowWidth = 0;
            int m_TargetWindowHeight = 0;

            int m_FrameDelay = 0;
            int m_FrameTime = 0;
            int m_FrameStart = 0;

            int m_DefaultVectorScaleWidth = 1600;
            int m_DefaultVectorScaleHeight = 900;

            bool m_DefaultVectorScale = true;

            std::string m_ParentFolder;

            std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> m_Window;
            std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> m_Renderer;

            std::queue<SDL_Event> m_EventQueue;
    };
}
