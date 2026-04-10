#include <pch.h>

#include <utils/logger.h>
#include <ui/UIContext.h>
#include <ui/UIManager.h>
#include <Scene.h>

using namespace engine::utils::logger;

namespace engine::ui {
    UIContext& getCurrentContext() {
        auto* scene = engine::SceneManager::getInstance().getCurrentSceneRaw();
        return scene->getUIContext();
    }

    namespace {
        static int s_ContextIndex = 0;

        int getRmlModifiers() {
            const SDL_Keymod sdlMods = SDL_GetModState();
            int mods = 0;

            if (sdlMods & SDL_KMOD_CTRL)   mods |= Rml::Input::KM_CTRL;
            if (sdlMods & SDL_KMOD_SHIFT)  mods |= Rml::Input::KM_SHIFT;
            if (sdlMods & SDL_KMOD_ALT)    mods |= Rml::Input::KM_ALT;
            if (sdlMods & SDL_KMOD_GUI)    mods |= Rml::Input::KM_META;
            if (sdlMods & SDL_KMOD_CAPS)   mods |= Rml::Input::KM_CAPSLOCK;
            if (sdlMods & SDL_KMOD_NUM)    mods |= Rml::Input::KM_NUMLOCK;
            if (sdlMods & SDL_KMOD_SCROLL) mods |= Rml::Input::KM_SCROLLLOCK;

            return mods;
        }

        Rml::Input::KeyIdentifier toRmlKey(SDL_Keycode key) {
            using KI = Rml::Input::KeyIdentifier;

            switch (key) {
                case SDLK_RETURN:    return KI::KI_RETURN;
                case SDLK_KP_ENTER:  return KI::KI_RETURN;
                case SDLK_ESCAPE:    return KI::KI_ESCAPE;
                case SDLK_BACKSPACE: return KI::KI_BACK;
                case SDLK_TAB:       return KI::KI_TAB;
                case SDLK_LEFT:      return KI::KI_LEFT;
                case SDLK_RIGHT:     return KI::KI_RIGHT;
                case SDLK_UP:        return KI::KI_UP;
                case SDLK_DOWN:      return KI::KI_DOWN;
                case SDLK_HOME:      return KI::KI_HOME;
                case SDLK_END:       return KI::KI_END;
                case SDLK_PAGEUP:    return KI::KI_PRIOR;
                case SDLK_PAGEDOWN:  return KI::KI_NEXT;
                case SDLK_INSERT:    return KI::KI_INSERT;
                case SDLK_DELETE:    return KI::KI_DELETE;
                case SDLK_F1:        return KI::KI_F1;
                case SDLK_F2:        return KI::KI_F2;
                case SDLK_F3:        return KI::KI_F3;
                case SDLK_F4:        return KI::KI_F4;
                case SDLK_F5:        return KI::KI_F5;
                case SDLK_F6:        return KI::KI_F6;
                case SDLK_F7:        return KI::KI_F7;
                case SDLK_F8:        return KI::KI_F8;
                case SDLK_F9:        return KI::KI_F9;
                case SDLK_F10:       return KI::KI_F10;
                case SDLK_F11:       return KI::KI_F11;
                case SDLK_F12:       return KI::KI_F12;
                default:             return KI::KI_UNKNOWN;
            }
        }
    }

    UIContext::~UIContext() {
        shutdown();
    }

    void UIContext::init(int width, int height) {
        if (m_Context) return;

        const std::string name = "ctx_" + std::to_string(s_ContextIndex++);
        m_Context = Rml::CreateContext(name, Rml::Vector2i{width, height});

        if (!m_Context) {
            Logger::engine_error("Rml::CreateContext({}) failed", name);
            return;
        }

        m_Context->SetDensityIndependentPixelRatio(UIManager::getInstance().getDpRatio());
    }

    void UIContext::shutdown() {
        if (m_Context) {
            Rml::RemoveContext(m_Context->GetName());
            m_Context = nullptr;
        }
    }

    void UIContext::resize(int width, int height, float dpRatio) {
        if (!m_Context) return;

        m_Context->SetDimensions(Rml::Vector2i{width, height});
        m_Context->SetDensityIndependentPixelRatio(dpRatio);
    }

    void UIContext::update() {
        if (m_Context) m_Context->Update();
    }

    void UIContext::render() {
        if (!m_Context) return;

        UIManager::getInstance().beginUIPass();
        m_Context->Render();
        UIManager::getInstance().endUIPass();
    }

    bool UIContext::isCapturingInput() const {
        if (!m_Context) return false;
        return m_Context->IsMouseInteracting();
    }

    UIDocument UIContext::loadDocument(const std::string& path) {
        if (!m_Context) {
            auto& ui = UIManager::getInstance();
            init(ui.getWidth(), ui.getHeight());
            if (!m_Context) return {};
        }

        Rml::ElementDocument* doc = m_Context->LoadDocument(path);

        if (!doc) {
            Logger::engine_error("Failed to load document: {}", path);
            return {};
        }

        doc->Show();
        Logger::engine_info("Loaded document: {}", path);

        return UIDocument(doc);
    }

    bool UIContext::processEvent(const SDL_Event& event) {
        if (!m_Context) return false;

        const int mods = getRmlModifiers();

        switch (event.type) {
            case SDL_EVENT_MOUSE_MOTION: {
                const bool notOver = m_Context->ProcessMouseMove(static_cast<int>(event.motion.x), static_cast<int>(event.motion.y), mods);
                return !notOver;
            }

            case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                m_Context->ProcessMouseMove(static_cast<int>(event.button.x), static_cast<int>(event.button.y), mods);

                const bool notConsumed = m_Context->ProcessMouseButtonDown(event.button.button - 1, mods);
                return !notConsumed;
            }

            case SDL_EVENT_MOUSE_BUTTON_UP: {
                m_Context->ProcessMouseMove(static_cast<int>(event.button.x), static_cast<int>(event.button.y), mods);

                const bool notConsumed = m_Context->ProcessMouseButtonUp(event.button.button - 1, mods);
                return !notConsumed;
            }

            case SDL_EVENT_MOUSE_WHEEL: {
                const bool notConsumed = m_Context->ProcessMouseWheel(Rml::Vector2f{event.wheel.x, -event.wheel.y}, mods);
                return !notConsumed;
            }

            case SDL_EVENT_KEY_DOWN: {
                const Rml::Input::KeyIdentifier rmlKey = toRmlKey(event.key.key);
                if (rmlKey != Rml::Input::KI_UNKNOWN) m_Context->ProcessKeyDown(rmlKey, mods);
                return isCapturingInput();
            }

            case SDL_EVENT_KEY_UP: {
                const Rml::Input::KeyIdentifier rmlKey = toRmlKey(event.key.key);
                if (rmlKey != Rml::Input::KI_UNKNOWN) m_Context->ProcessKeyUp(rmlKey, mods);
                return isCapturingInput();
            }

            case SDL_EVENT_TEXT_INPUT: {
                if (isCapturingInput()) {
                    m_Context->ProcessTextInput(Rml::String(event.text.text));
                    return true;
                }

                return false;
            }

            default:
                return false;
        }
    }
}
