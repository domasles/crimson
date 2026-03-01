#include <pch.h>

#include <ui/UISystemInterface.h>
#include <utils/logger.h>

using namespace engine::utils::logger;

namespace engine::ui {
    double UISystemInterface::GetElapsedTime() {
        double nowSeconds = static_cast<double>(SDL_GetTicksNS()) * 1e-9;

        if (m_StartTimeSeconds < 0.0) {
            m_StartTimeSeconds = nowSeconds;
        }

        return nowSeconds - m_StartTimeSeconds;
    }

    bool UISystemInterface::LogMessage(Rml::Log::Type type, const Rml::String& message) {
        switch (type) {
            case Rml::Log::LT_ERROR:
            case Rml::Log::LT_ASSERT:
                Logger::engine_error("{}", message);
                break;
            case Rml::Log::LT_WARNING:
                Logger::engine_warn("{}", message);
                break;
            default:
                Logger::engine_debug("{}", message);
                break;
        }

        return true;
    }
}
