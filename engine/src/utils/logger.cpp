#include <pch.h>

#include <utils/logger.h>

namespace engine::utils::logger {
    void Logger::info(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);

        SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, args);
        
        va_end(args);
    }

    void Logger::warn(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);

        SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, fmt, args);

        va_end(args);
    }

    void Logger::error(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);

        SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, fmt, args);

        va_end(args);
    }

    void Logger::debug(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);

        SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, fmt, args);

        va_end(args);
    }
}
