#include <pch.h>

#include <utils/logger.h>
#include <utils/math.h>

#include <ui/UIManager.h>

#include <Gizmos.h>
#include <Scene.h>
#include <Core.h>

#ifdef ENGINE_PLATFORM_EMSCRIPTEN
    #include <emscripten.h>
#endif

using namespace engine::utils::logger;
using namespace engine::utils::math;
using namespace engine::ui;
using namespace engine;

#ifdef ENGINE_PLATFORM_EMSCRIPTEN
    static std::function<void()> s_WASMUpdate;

    static void emscripten_main_loop() {
        Core& core = Core::getInstance();

        if (!core.processEvents()) {
            emscripten_cancel_main_loop();
            return;
        }

        if (core.getUseCustomOutOfBoundsColor()) {
            int windowWidth, windowHeight;
            SDL_GetWindowSize(core.getWindow(), &windowWidth, &windowHeight);

            glDisable(GL_SCISSOR_TEST);
            glViewport(0, 0, windowWidth, windowHeight);

            core.getRenderer()->clear(core.getOutOfBoundsColor());
            core.updateViewport();
        }

        if (core.getWindowResized()) {
            core.updateViewport();
        }

        core.getRenderer()->clear(getCore().getBackgroundColor());

        if (s_WASMUpdate) {
            s_WASMUpdate();
        }

        SceneManager::getInstance().update();
        SceneManager::getInstance().prepareRender();

        // PASS 1: Render all opaque game objects
        core.getRenderer()->beginPass(RenderPass::Opaque);
        SceneManager::getInstance().render();
        core.getRenderer()->endPass();

        // PASS 2: Render all debug gizmos
        core.getRenderer()->beginPass(RenderPass::Debug);
        Gizmos::renderGizmos();
        core.getRenderer()->endPass();

        // PASS 3: UI overlay
        core.getRenderer()->beginPass(RenderPass::UI);
        SceneManager::getInstance().updateUI();
        SceneManager::getInstance().renderUI();
        core.getRenderer()->endPass();

        core.getRenderer()->endFrame();
        SDL_GL_SwapWindow(core.getWindow());
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
        for (MIX_Track* track : m_Tracks) {
            if (track) {
                MIX_DestroyTrack(track);
            }
        }

        m_Tracks.clear();

        if (m_Mixer) {
            MIX_DestroyMixer(m_Mixer);
            m_Mixer = nullptr;
        }

        UIManager::getInstance().shutdown();
        m_Renderer->shutdown();
        
        if (m_GLContext) {
            SDL_GL_DestroyContext(m_GLContext);
            m_GLContext = nullptr;
        }

        MIX_Quit();
        SDL_Quit();
    }

    bool Core::init(const std::string& workingDir, const std::string& title, const int width, const int height, const bool resizable, const bool vsync) {
        m_VSync = vsync;
        return initInternal(workingDir, title, WindowMode::Windowed, width, height, resizable, vsync);
    }

    bool Core::init(const std::string& workingDir, const std::string& title, const bool fullScreen, const bool vsync) {
        if (!fullScreen) {
            Logger::engine_error("Flag 'fullscreen' must be set to true!");
            return false;
        }

        m_VSync = vsync;
        return initInternal(workingDir, title, WindowMode::Fullscreen, 0, 0, false, vsync);
    }

    bool Core::initInternal(const std::string& workingDir, const std::string& title, WindowMode mode, int width, int height, bool resizable, bool vsync) {
        m_ParentFolder = workingDir;
        m_VSync = vsync;

        SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE);

        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
            Logger::engine_error("SDL_Init failed: {}", SDL_GetError());
            return false;
        }

        ENGINE_LOG_INIT("SDL");

        if (!MIX_Init()) {
            Logger::engine_error("MIX_Init failed: {}", SDL_GetError());
            return false;
        }

        m_Mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

        if (!m_Mixer) {
            Logger::engine_error("MIX_CreateMixerDevice failed: {}", SDL_GetError());
            return false;
        }

        m_Tracks.reserve(32);

        for (int i = 0; i < 32; ++i) {
            MIX_Track* track = MIX_CreateTrack(m_Mixer);
            if (track) {
                m_Tracks.push_back(track);
            }
        }

        ENGINE_LOG_INIT("Audio");

        bool windowSuccess = false;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        switch (mode) {
            case WindowMode::Windowed:
                windowSuccess = initWindowedWindow(title, width, height, resizable);

                if (windowSuccess) {
                    m_TargetWindowWidth = width;
                    m_TargetWindowHeight = height;

                    ENGINE_LOG_INIT("Windowed Window");
                }

                break;

            case WindowMode::Fullscreen:
                windowSuccess = initFullScreenWindow(title);

                if (windowSuccess) {
                    Vector2 windowSize = getWindowSize();

                    m_TargetWindowWidth = m_DefaultWindowWidth;
                    m_TargetWindowHeight = m_DefaultWindowHeight;

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
        UIManager::getInstance().init(m_Renderer.get(), m_TargetWindowWidth, m_TargetWindowHeight);

        return true;
    }

    bool Core::processEvents() {
        SDL_Event event;
        m_WindowResized = false;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                return false;
            }

            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                m_WindowResized = true;
            }

            SceneManager::getInstance().processUIEvent(event);
        }

        return true;
    }

    void Core::run(std::function<void()> customUpdate) {
        #ifdef ENGINE_PLATFORM_EMSCRIPTEN
            s_WASMUpdate = customUpdate;
            emscripten_set_main_loop(emscripten_main_loop, 0, 1);
        #else
            while (processEvents()) {
                if (m_UseCustomOutOfBoundsColor) {
                    int windowWidth, windowHeight;
                    SDL_GetWindowSize(m_Window.get(), &windowWidth, &windowHeight);

                    glDisable(GL_SCISSOR_TEST);
                    glViewport(0, 0, windowWidth, windowHeight);

                    m_Renderer->clear(m_OutOfBoundsColor);
                    updateViewport();
                }

                if (m_WindowResized) {
                    updateViewport();
                }

                m_Renderer->clear(m_BackgroundColor);

                if (customUpdate) {
                    customUpdate();
                }

                SceneManager::getInstance().update();
                SceneManager::getInstance().updateUI();
                SceneManager::getInstance().prepareRender();

                // PASS 1: Render all opaque game objects
                m_Renderer->beginPass(RenderPass::Opaque);
                SceneManager::getInstance().render();
                m_Renderer->endPass();

                // PASS 2: Render all debug gizmos
                m_Renderer->beginPass(RenderPass::Debug);
                Gizmos::renderGizmos();
                m_Renderer->endPass();

                // PASS 3: UI overlay
                m_Renderer->beginPass(RenderPass::UI);
                SceneManager::getInstance().renderUI();
                m_Renderer->endPass();

                m_Renderer->endFrame();
                SDL_GL_SwapWindow(m_Window.get());
            }

            UIManager::getInstance().shutdown();
        #endif
    }

    void Core::setBackgroundColor(const Color& color) {
        m_BackgroundColor = color;
    }

    void Core::setOutOfBoundsColor(bool custom, const Color& color) {
        m_UseCustomOutOfBoundsColor = custom;
        m_OutOfBoundsColor = color;
    }

    void Core::setVectorScale(int targetWindowWidth, int targetWindowHeight) {
        m_TargetWindowWidth = targetWindowWidth;
        m_TargetWindowHeight = targetWindowHeight;
    }

    void Core::setVectorScale(bool useDefaultScale) {
        m_DefaultVectorScale = useDefaultScale;
    }

    GLRenderer* Core::getRenderer() const {
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

    Vector2 Core::getWindowSize() {
        SDL_GetWindowSize(getWindow(), &m_WindowWidth, &m_WindowHeight);

        float w = std::max(m_WindowWidth, m_TargetWindowWidth);
        float h = std::max(m_WindowHeight, m_TargetWindowHeight);

        return Vector2{ static_cast<float>(w), static_cast<float>(h) };
    }

    Vector2 Core::getLogicalWindowSize() {
        return Vector2{
            static_cast<float>(m_TargetWindowWidth),
            static_cast<float>(m_TargetWindowHeight)
        };
    }

    bool Core::initWindowedWindow(const std::string& title, const int width, const int height, const bool resizable) {
        #ifdef ENGINE_PLATFORM_EMSCRIPTEN
            m_Window = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>(SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE), SDL_DestroyWindow);
        #else
            m_Window = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>(SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_OPENGL | (resizable ? SDL_WINDOW_RESIZABLE : 0)), SDL_DestroyWindow);
        #endif

        if (!m_Window) {
            Logger::engine_error("SDL_CreateWindow failed: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    bool Core::initFullScreenWindow(const std::string& title) {
        m_Window = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>(SDL_CreateWindow(title.c_str(), m_DefaultWindowWidth, m_DefaultWindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN), SDL_DestroyWindow);
      
        if (!m_Window) {
            Logger::engine_error("SDL_CreateWindow failed: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    bool Core::initRenderer() {
        m_GLContext = SDL_GL_CreateContext(m_Window.get());

        if (!m_GLContext) {
            Logger::engine_error("SDL_GL_CreateContext failed: {}", SDL_GetError());
            return false;
        }

        #ifndef ENGINE_PLATFORM_EMSCRIPTEN
            if (!SDL_GL_SetSwapInterval(m_VSync ? 1 : 0)) {
                Logger::engine_warn("SDL_GL_SetSwapInterval failed: {}", SDL_GetError());
            }
        #endif

        m_Renderer = std::make_unique<GLRenderer>();

        if (!m_Renderer->init()) {
            Logger::engine_error("GLRenderer initialization failed");
            return false;
        }

        int windowWidth = 0;
        int windowHeight = 0;

        SDL_GetWindowSize(m_Window.get(), &windowWidth, &windowHeight);

        m_Renderer->setViewport(0, 0, windowWidth, windowHeight);
        m_Renderer->setOrthographicProjection(0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight), 0.0f);

        m_WindowResized = true;
        updateViewport();
        
        return true;
    }

    void Core::updateViewport() {
        int windowWidth = 0;
        int windowHeight = 0;

        SDL_GetWindowSize(m_Window.get(), &windowWidth, &windowHeight);

        float virtualWidth = 0.0f;
        float virtualHeight = 0.0f;

        if (m_DefaultVectorScale) {
            virtualWidth = static_cast<float>(m_DefaultWindowWidth);
            virtualHeight = static_cast<float>(m_DefaultWindowHeight);
        }

        else {
            virtualWidth = static_cast<float>(m_TargetWindowWidth);
            virtualHeight = static_cast<float>(m_TargetWindowHeight);
        }

        // Calculate scale to maintain aspect ratio (letterboxing/pillarboxing)
        float scaleX = static_cast<float>(windowWidth) / virtualWidth;
        float scaleY = static_cast<float>(windowHeight) / virtualHeight;
        float scale = std::min(scaleX, scaleY);

        int viewportWidth = static_cast<int>(virtualWidth * scale);
        int viewportHeight = static_cast<int>(virtualHeight * scale);
        int viewportX = (windowWidth - viewportWidth) / 2;
        int viewportY = (windowHeight - viewportHeight) / 2;

        m_Renderer->setViewport(viewportX, viewportY, viewportWidth, viewportHeight);
        m_Renderer->setOrthographicProjection(0.0f, virtualWidth, virtualHeight, 0.0f);
    }

    float Core::calculateUniformScale(int width, int height, float baseWidth, float baseHeight) {
        float scaleX = static_cast<float>(width) / baseWidth;
        float scaleY = static_cast<float>(height) / baseHeight;

        return std::min(scaleX, scaleY);
    }

    MIX_Track* Core::getFreeTrack() {
        for (MIX_Track* track : m_Tracks) {
            if (track && !MIX_TrackPlaying(track)) {
                return track;
            }
        }
        return m_Tracks.empty() ? nullptr : m_Tracks[0];
    }

    void Core::setVSync(bool enabled) {
        m_VSync = enabled;
        
        #ifndef ENGINE_PLATFORM_EMSCRIPTEN
            if (!SDL_GL_SetSwapInterval(m_VSync ? 1 : 0)) {
                Logger::engine_warn("SDL_GL_SetSwapInterval failed: {}", SDL_GetError());
            }
        #endif
    }

    void Core::setFullscreen(bool enabled) {
        if (!m_Window) {
            Logger::engine_error("Cannot set fullscreen: window not initialized");
            return;
        }

        if (!SDL_SetWindowFullscreen(m_Window.get(), enabled ? SDL_WINDOW_FULLSCREEN : 0)) {
            Logger::engine_error("SDL_SetWindowFullscreen failed: {}", SDL_GetError());
        }
    }
}
