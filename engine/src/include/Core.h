#pragma once

#ifdef GAME_PLATFORM_WINDOWS
    #define GAME_API extern "C" __declspec(dllexport)
#elif defined(GAME_PLATFORM_LINUX) || defined(GAME_PLATFORM_MACOS)
    #define GAME_API extern "C" __attribute__((visibility("default")))
#else
    #define GAME_API extern "C"
#endif

#ifdef ENGINE_PLATFORM_WINDOWS
    #define ENGINE_API extern "C" __declspec(dllexport)
#elif defined(ENGINE_PLATFORM_LINUX) || defined(ENGINE_PLATFORM_MACOS)
    #define ENGINE_API extern "C" __attribute__((visibility("default")))
#else
    #define ENGINE_API extern "C"
#endif

#include <utils/rendering.h>

#include <GLRenderer.h>
#include <Input.h>

using namespace engine::utils::rendering;

namespace engine {
    class Core {
        public:
            static Core& getInstance();

            MIX_Mixer* getMixer() const { return m_Mixer; }
            MIX_Track* getFreeTrack();

            bool init(const std::string& workingDir, const std::string& title, const int width=800, const int height=600, const bool resizable=false, const bool vsync=true);
            bool init(const std::string& workingDir, const std::string& title, const bool fullScreen=false, const bool vsync=true);
            bool initInternal(const std::string& workingDir, const std::string& title, WindowMode mode, int width = 0, int height = 0, bool resizable = false, bool vsync = true);

            bool processEvents();
            const std::string& getName() const { return m_ParentFolder; }

            void run(std::function<void()> customUpdate);

            void setVSync(bool enabled);
            bool getVSync() const { return m_VSync; }

            void setFullscreen(bool enabled);

            void setVectorScale(int targetWindowWidth, int targetWindowHeight);
            void setVectorScale(bool useDefaultScale=true) { m_DefaultVectorScale = useDefaultScale;}

            void updateViewport();

            int getLetterboxX() const { return m_LetterboxX; }
            int getLetterboxY() const { return m_LetterboxY; }
            float getLetterboxScale() const { return m_LetterboxScale; }

            GLRenderer* getRenderer() const;
            SDL_Window* getWindow() const;

            Vector2 getWindowSize();
            Vector2 getTargetWindowSize() const { return Vector2{static_cast<float>(m_TargetWindowWidth), static_cast<float>(m_TargetWindowHeight)}; }
            Vector2 getLogicalWindowSize();

        private:
            Core() : m_Window(nullptr, SDL_DestroyWindow) {}
            ~Core();

            Core(const Core&) = delete;
            Core& operator=(const Core&) = delete;

            bool initWindowedWindow(const std::string& title, const int width, const int height, const bool resizable);
            bool initFullScreenWindow(const std::string& title);
            bool initRenderer();

            float calculateUniformScale(int width, int height, float baseWidth, float baseHeight);

            int m_WindowWidth = 0;
            int m_WindowHeight = 0;

            int m_DefaultWindowWidth = 1600;
            int m_DefaultWindowHeight = 900;

            int m_TargetWindowWidth = 0;
            int m_TargetWindowHeight = 0;

            bool m_DefaultVectorScale = true;
            bool m_VSync = true;

            int m_LetterboxX = 0;
            int m_LetterboxY = 0;
            float m_LetterboxScale = 1.0f;

            std::string m_ParentFolder;

            std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> m_Window;
            std::unique_ptr<GLRenderer> m_Renderer;

            SDL_GLContext m_GLContext = nullptr;

            MIX_Mixer* m_Mixer = nullptr;
            std::vector<MIX_Track*> m_Tracks;
    };

    inline Core& getCore() { return Core::getInstance(); }

    inline GLRenderer* getRenderer() { return getCore().getRenderer(); }
    inline SDL_Window* getWindow() { return getCore().getWindow(); }

    inline Vector2 getWindowSize() { return getCore().getWindowSize(); }
    inline Vector2 getTargetWindowSize() { return getCore().getTargetWindowSize(); }
    inline Vector2 getLogicalWindowSize() { return getCore().getLogicalWindowSize(); }
}
