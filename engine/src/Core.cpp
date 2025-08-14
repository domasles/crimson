#include <pch.h>

#include <utils/logger.h>
#include <utils/math.h>

#include <Scene.h>
#include <Core.h>

#ifdef ENGINE_PLATFORM_EMSCRIPTEN
    #include <emscripten.h>
#endif

using namespace engine::utils::logger;
using namespace engine::utils::math;
using namespace engine;

#ifdef ENGINE_PLATFORM_EMSCRIPTEN
    static std::function<void()> s_WASMUpdate;

    static void emscripten_main_loop() {
        Core& core = Core::getInstance();

        if (!core.processEvents()) {
            emscripten_cancel_main_loop();
            return;
        }

        if (!SDL_RenderClear(core.getRenderer())) {
            Logger::engine_error("SDL_RenderClear failed: {}", SDL_GetError());
        }

        core.updateVectorScale();

        if (s_WASMUpdate) {
            s_WASMUpdate();
        }

        SceneManager::getInstance().update();
        SceneManager::getInstance().render();

        SDL_RenderPresent(core.getRenderer());
    }
#endif

ENGINE_API void internalUpdate(std::function<void()> customUpdate) {
    engine::Core::getInstance().run(customUpdate);
}

namespace engine {
    Core& Core::getInstance() {
        try {
            static Core& instance = *new Core();
            return instance;
        }
        
        catch (const std::bad_alloc& e) {
            Logger::engine_error("Memory allocation failed: {}", e.what());
        }

        static Core fallbackInstance;
        return fallbackInstance;
    }

    Core::~Core() {
        Mix_CloseAudio();
        Mix_Quit();
        SDL_Quit();
    }

    const bool Core::init(const std::string& workingDir, const std::string& title, const int width, const int height, const bool resizable) {
        return initInternal(workingDir, title, WindowMode::Windowed, width, height, resizable);
    }

    const bool Core::init(const std::string& workingDir, const std::string& title, const bool fullScreen) {
        if (!fullScreen) {
            Logger::engine_error("Flag 'fullscreen' must be set to true!");
            return false;
        }
        return initInternal(workingDir, title, WindowMode::Fullscreen);
    }

    const bool Core::initInternal(const std::string& workingDir, const std::string& title, WindowMode mode, int width, int height, bool resizable) {
        m_ParentFolder = workingDir;

        SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE);

        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
            Logger::engine_error("SDL_Init failed: {}", SDL_GetError());
            return false;
        }

        ENGINE_LOG_INIT("SDL");

        const int mixFlags = MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_FLAC;

        if ((Mix_Init(mixFlags) & mixFlags) != mixFlags) {
            Logger::engine_error("Mix_Init missing codec support: {}", SDL_GetError());
            return false;
        }

        if (!Mix_OpenAudio(0, nullptr)) {
            Logger::engine_error("Mix_OpenAudio failed: {}", SDL_GetError());
            return false;
        }

        Mix_AllocateChannels(32);
        ENGINE_LOG_INIT("Audio");

        bool windowSuccess = false;

        switch (mode) {
            case WindowMode::Windowed:
                windowSuccess = initWindowedWindow(title, width, height, resizable);

                if (windowSuccess) {
                    m_WindowWidth = m_TargetWindowWidth = width;
                    m_WindowHeight = m_TargetWindowHeight = height;

                    ENGINE_LOG_INIT("Windowed Window");
                }

                break;

            case WindowMode::Fullscreen:
                windowSuccess = initFullScreenWindow(title);

                if (windowSuccess) {
                    SDL_GetWindowSize(getWindow(), &m_WindowWidth, &m_WindowHeight);

                    m_TargetWindowWidth = m_WindowWidth;
                    m_TargetWindowHeight = m_WindowHeight;

                    ENGINE_LOG_INIT("Fullscreen Window");
                }

                break;
        }

        if (!windowSuccess) {
            Logger::engine_error("Window initialization failed");
            return false;
        }

        if (!initRenderer()) {
            Logger::engine_error("initRenderer failed");
            return false;
        }

        ENGINE_LOG_INIT("Renderer");
        return true;
    }

    const bool Core::processEvents() {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            m_EventQueue.push(event);
        }

        while (!m_EventQueue.empty()) {
            SDL_Event e = m_EventQueue.front();
            m_EventQueue.pop();

            if (e.type == SDL_EVENT_QUIT) {
                return false;
            }
        }

        return true;
    }

    void Core::run(std::function<void()> customUpdate) {
        #ifdef ENGINE_PLATFORM_EMSCRIPTEN
            s_WASMUpdate = customUpdate;
            emscripten_set_main_loop(emscripten_main_loop, 0, 1);
        #else
            while (processEvents()) {
                if (!SDL_RenderClear(getRenderer())) {
                    Logger::engine_error("SDL_RenderClear failed: {}", SDL_GetError());
                }

                updateVectorScale();

                if (customUpdate) {
                    customUpdate();
                }

                SceneManager::getInstance().update();
                SceneManager::getInstance().render();

                SDL_RenderPresent(getRenderer());
            }
        #endif
    }

    void Core::setVectorScale(int targetWindowWidth, int targetWindowHeight) {
        m_TargetWindowWidth = targetWindowWidth;
        m_TargetWindowHeight = targetWindowHeight;
    }

    void Core::setVectorScale(bool useDefaultScale) {
        m_DefaultVectorScale = useDefaultScale;
    }

    SDL_Renderer* Core::getRenderer() const {
        if (!m_Renderer) {
            Logger::engine_error("Renderer is not initialized yet!");
            return nullptr;
        }

        return m_Renderer.get();
    }

    SDL_Window* Core::getWindow() const {
        if (!m_Window) {
            Logger::engine_error("Window is not initialized yet!");
            return nullptr;
        }

        return m_Window.get();
    }
    
    bool Core::initWindowedWindow(const std::string& title, const int width, const int height, const bool resizable) {
        #ifdef ENGINE_PLATFORM_EMSCRIPTEN
            m_Window = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>(SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_RESIZABLE), SDL_DestroyWindow);
        #else
            m_Window = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>(SDL_CreateWindow(title.c_str(), width, height, resizable ? SDL_WINDOW_RESIZABLE : SDL_WINDOW_EXTERNAL), SDL_DestroyWindow);
        #endif

        if (!m_Window) {
            Logger::engine_error("SDL_CreateWindow failed: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    bool Core::initFullScreenWindow(const std::string& title) {
        m_Window = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>(SDL_CreateWindow(title.c_str(), 1600, 900, true), SDL_DestroyWindow);
      
        if (!m_Window) {
            Logger::engine_error("SDL_CreateWindow failed: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    bool Core::initRenderer() {
        m_Renderer = std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)>(SDL_CreateRenderer(m_Window.get(), nullptr), SDL_DestroyRenderer);

        if (!m_Renderer) {
            Logger::engine_error("SDL_CreateRenderer failed: {}", SDL_GetError());
            return false;
        }
        
#ifndef ENGINE_PLATFORM_EMSCRIPTEN
        // VSync can interfere with Emscripten's main loop timing
        if (!SDL_SetRenderVSync(getRenderer(), true)) {
            Logger::engine_error("SDL_SetRenderVSync failed: {}", SDL_GetError());
        }
#endif
        
        return true;
    }

    void Core::updateVectorScale() {
        int prevWidth = 0;
        int prevHeight = 0;

        SDL_GetCurrentRenderOutputSize(getRenderer(), &prevWidth, &prevHeight);

        float baseWidth = 0.0f;
        float baseHeight = 0.0f;

        if (m_DefaultVectorScale) {
            baseWidth = static_cast<float>(m_DefaultVectorScaleWidth);
            baseHeight = static_cast<float>(m_DefaultVectorScaleHeight);
        }

        else {
            baseWidth = static_cast<float>(m_TargetWindowWidth);
            baseHeight = static_cast<float>(m_TargetWindowHeight);
        }

        float initialScale = calculateUniformScale(prevWidth, prevHeight, baseWidth, baseHeight);
        
        Vector2::setGlobalScale(initialScale, initialScale);
        Vector2::updateAll();

        int currentWidth = 0;
        int currentHeight = 0;

        SDL_GetCurrentRenderOutputSize(getRenderer(), &currentWidth, &currentHeight);

        if (currentWidth != prevWidth || currentHeight != prevHeight) {
            float updatedScale = calculateUniformScale(currentWidth, currentHeight, baseWidth, baseHeight);

            Vector2::setGlobalScale(updatedScale, updatedScale);
            Vector2::updateAll();
        }
    }

    float Core::calculateUniformScale(int width, int height, float baseWidth, float baseHeight) {
        float scaleX = static_cast<float>(width) / baseWidth;
        float scaleY = static_cast<float>(height) / baseHeight;

        return std::min(scaleX, scaleY);
    }
}
