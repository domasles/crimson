#include <pch.h>

#include <utils/math.h>

#include <Scene.h>
#include <Core.h>

using namespace engine::utils::math;

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
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Memory allocation failed: %s", e.what());
        }

        static Core fallbackInstance;
        return fallbackInstance;
    }

    const bool Core::init(const std::string& workingDir, const std::string& title, const int width, const int height, const bool resizable) {
        m_ParentFolder = workingDir;

        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed: %s", SDL_GetError());
            return false;
        }

        initWindowedWindow(title, width, height, resizable);

        m_WindowWidth = m_TargetWindowWidth = width;
        m_WindowHeight = m_TargetWindowHeight = height;

        initRenderer();

        return true;
    }

    const bool Core::init(const std::string& workingDir, const std::string& title, const bool fullScreen) {
        if (!fullScreen) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Flag 'fullscreen' must be set to true!");
            return false;
        }

        m_ParentFolder = workingDir;

        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed: %s", SDL_GetError());
            return false;
        }

        initFullScreenWindow(title);
        SDL_GetWindowSize(getWindow(), &m_WindowWidth, &m_WindowHeight);
        initRenderer();

        m_TargetWindowWidth = m_WindowWidth;
        m_TargetWindowHeight = m_WindowHeight;

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
        while (processEvents()) {
            SDL_RenderClear(getRenderer());
            updateVectorScale();

            if (customUpdate) {
                customUpdate();
            }

            SceneManager::getInstance().update();
            SceneManager::getInstance().render();

            SDL_RenderPresent(getRenderer());
        }
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
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer is not initialized yet!");
            return nullptr;
        }

        return m_Renderer.get();
    }

    SDL_Window* Core::getWindow() const {
        if (!m_Window) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window is not initialized yet!");
            return nullptr;
        }

        return m_Window.get();
    }

    void Core::initWindowedWindow(const std::string& title, const int width, const int height, const bool resizable) {
        m_Window = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>(SDL_CreateWindow(title.c_str(), width, height, resizable ? SDL_WINDOW_RESIZABLE : SDL_WINDOW_EXTERNAL), SDL_DestroyWindow);

        if (!m_Window) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow failed: %s", SDL_GetError());
        }
    }

    void Core::initFullScreenWindow(const std::string& title) {
        m_Window = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>(SDL_CreateWindow(title.c_str(), 0, 0, true), SDL_DestroyWindow);

        if (!m_Window) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow failed: %s", SDL_GetError());
        }
    }

    void Core::initRenderer() {
        m_Renderer = std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)>(SDL_CreateRenderer(m_Window.get(), nullptr), SDL_DestroyRenderer);

        if (!m_Renderer) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer failed: %s", SDL_GetError());
        }

        if (!SDL_SetRenderVSync(getRenderer(), true)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_SetRenderVSync failed: %s", SDL_GetError());
        }
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
