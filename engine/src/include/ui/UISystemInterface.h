#pragma once

#include <RmlUi/Core/SystemInterface.h>

namespace engine::ui {
    class UISystemInterface : public Rml::SystemInterface {
        public:
            UISystemInterface() = default;
            ~UISystemInterface() override = default;

            double GetElapsedTime() override;
            bool LogMessage(Rml::Log::Type type, const Rml::String& message) override;

        private:
            double m_StartTimeSeconds = -1.0;
    };
}
