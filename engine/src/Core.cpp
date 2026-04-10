#include <pch.h>

#include <utils/logger.h>
#include <utils/math.h>

#include <ui/UIManager.h>

#include <AudioManager.h>
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
    static void emscripten_main_loop() {
        Core& core = Core::getInstance();

        if (!core.processEvents()) {
            emscripten_cancel_main_loop();
            return;
        }

        core.runFrame();
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
        AudioManager::getInstance().shutdown();
        UIManager::getInstance().shutdown();

        m_Renderer->shutdown();
        
        if (m_GLContext) {
            SDL_GL_DestroyContext(m_GLContext);
            m_GLContext = nullptr;
        }

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

        if (!AudioManager::getInstance().init()) {
            Logger::engine_error("AudioManager initialization failed");
            return false;
        }

        ENGINE_LOG_INIT("Audio");

        bool windowSuccess = false;

        #ifdef ENGINE_PLATFORM_EMSCRIPTEN
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        #else
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        #endif

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

        {
            int physW = 0, physH = 0;
            SDL_GetWindowSize(m_Window.get(), &physW, &physH);

            m_WindowWidth = physW;
            m_WindowHeight = physH;

            UIManager::getInstance().init(m_Renderer.get(), physW, physH);
        }

        updateViewport();

        return true;
    }

    bool Core::processEvents() {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                return false;
            }

            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                m_WindowWidth = event.window.data1;
                m_WindowHeight = event.window.data2;

                updateViewport();
            }

            SceneManager::getInstance().processUIEvent(event);
        }

        return true;
    }

    void Core::run(std::function<void()> customUpdate) {
        m_CustomUpdate = std::move(customUpdate);

        #ifdef ENGINE_PLATFORM_EMSCRIPTEN
            emscripten_set_main_loop(emscripten_main_loop, 0, 1);
        #else
            while (processEvents()) runFrame();
            UIManager::getInstance().shutdown();
        #endif
    }

    void Core::runFrame() {
        glDisable(GL_SCISSOR_TEST);
        glViewport(0, 0, m_WindowWidth, m_WindowHeight);

        m_Renderer->clear(SceneManager::getInstance().getOutOfBoundsColor());

        applyLetterboxViewport();
        m_Renderer->clear(SceneManager::getInstance().getBackgroundColor());

        if (m_CustomUpdate) m_CustomUpdate();

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

        SDL_GL_SwapWindow(m_Window.get());
    }

    void Core::applyLetterboxViewport() {
        m_Renderer->setViewport(m_LetterboxX, m_LetterboxY, m_ViewportW, m_ViewportH);
    }

    void Core::setVectorScale(int targetWindowWidth, int targetWindowHeight) {
        m_TargetWindowWidth = targetWindowWidth;
        m_TargetWindowHeight = targetWindowHeight;
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

        int windowWidth = m_WindowWidth;
        int windowHeight = m_WindowHeight;

        m_Renderer->setViewport(0, 0, windowWidth, windowHeight);
        m_Renderer->setOrthographicProjection(0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight), 0.0f);
        
        return true;
    }

    void Core::updateViewport() {
        int windowWidth = m_WindowWidth;
        int windowHeight = m_WindowHeight;

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

        float scaleX = static_cast<float>(windowWidth) / virtualWidth;
        float scaleY = static_cast<float>(windowHeight) / virtualHeight;
        float scale = std::min(scaleX, scaleY);

        int viewportWidth = static_cast<int>(virtualWidth * scale);
        int viewportHeight = static_cast<int>(virtualHeight * scale);
        int viewportX = (windowWidth - viewportWidth) / 2;
        int viewportY = (windowHeight - viewportHeight) / 2;

        m_LetterboxX = viewportX;
        m_LetterboxY = viewportY;
        m_LetterboxScale = scale;
        m_ViewportW = viewportWidth;
        m_ViewportH = viewportHeight;

        m_Renderer->setViewport(viewportX, viewportY, viewportWidth, viewportHeight);
        m_Renderer->setOrthographicProjection(0.0f, virtualWidth, virtualHeight, 0.0f);

        UIManager::getInstance().updateSize(windowWidth, windowHeight, scale);
        SceneManager::getInstance().resizeUI(windowWidth, windowHeight, scale);
    }

    float Core::calculateUniformScale(int width, int height, float baseWidth, float baseHeight) {
        float scaleX = static_cast<float>(width) / baseWidth;
        float scaleY = static_cast<float>(height) / baseHeight;

        return std::min(scaleX, scaleY);
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
