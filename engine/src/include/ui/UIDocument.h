#pragma once

namespace Rml {
    class ElementDocument;
}

namespace engine::ui {
    class UIDocument {
        public:
            UIDocument() = default;
            explicit UIDocument(Rml::ElementDocument* doc);

            ~UIDocument();

            UIDocument(UIDocument&&) noexcept;
            UIDocument& operator=(UIDocument&&) noexcept;

            UIDocument(const UIDocument&) = delete;
            UIDocument& operator=(const UIDocument&) = delete;

            UIDocument& listen(const std::string& elementId, const std::string& event, std::function<void()> callback);

            void show();
            void hide();

            explicit operator bool() const { return m_Doc != nullptr; }

        private:
            Rml::ElementDocument* m_Doc = nullptr;
            std::vector<std::shared_ptr<void>> m_Listeners;
    };
}
