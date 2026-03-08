#include <pch.h>

#include <ui/UIDocument.h>
#include <ui/UIContext.h>

#include <utils/logger.h>

using namespace engine::utils::logger;

namespace engine::ui {
    namespace {
        class FuncEventListener : public Rml::EventListener {
            public:
                explicit FuncEventListener(std::function<void()> fn) : m_Fn(std::move(fn)) {}

                void ProcessEvent(Rml::Event&) override {
                    if (m_Fn) m_Fn();
                }

            private:
                std::function<void()> m_Fn;
        };
    }

    UIDocument::UIDocument(Rml::ElementDocument* doc) : m_Doc(doc) {}

    UIDocument::~UIDocument() {
        if (m_Doc) {
            m_Doc->GetContext()->UnloadDocument(m_Doc);
            m_Doc = nullptr;
        }
    }

    UIDocument::UIDocument(UIDocument&& other) noexcept : m_Doc(other.m_Doc), m_Listeners(std::move(other.m_Listeners)) {
        other.m_Doc = nullptr;
    }

    UIDocument& UIDocument::operator=(UIDocument&& other) noexcept {
        if (this != &other) {
            if (m_Doc) m_Doc->GetContext()->UnloadDocument(m_Doc);

            m_Doc = other.m_Doc;
            m_Listeners = std::move(other.m_Listeners);

            other.m_Doc = nullptr;
        }

        return *this;
    }

    UIDocument& UIDocument::listen(const std::string& elementId, const std::string& event, std::function<void()> callback) {
        if (!m_Doc) return *this;

        Rml::Element* el = m_Doc->GetElementById(elementId);

        if (!el) {
            Logger::engine_warn("UIDocument::listen: element '{}' not found.", elementId);
            return *this;
        }

        auto listener = std::make_shared<FuncEventListener>(std::move(callback));

        el->AddEventListener(event, listener.get());
        m_Listeners.push_back(std::move(listener));

        return *this;
    }

    UIDocument& UIDocument::setDocument(const std::string& path) {
        *this = getCurrentContext().loadDocument(getGamePath() + "/" + path);
        return *this;
    }

    void UIDocument::show() {
        if (m_Doc) m_Doc->Show();
    }

    void UIDocument::hide() {
        if (m_Doc) m_Doc->Hide();
    }
}
