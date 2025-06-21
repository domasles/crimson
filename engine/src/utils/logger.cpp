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

    void Logger::engine_info(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);

        char buffer[1024];

        snprintf(buffer, sizeof(buffer), "ENGINE: %s", fmt);
        SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, buffer, args);
        
        va_end(args);
    }

    void Logger::engine_warn(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);

        char buffer[1024];

        snprintf(buffer, sizeof(buffer), "ENGINE: %s", fmt);
        SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, buffer, args);

        va_end(args);
    }

    void Logger::engine_error(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);

        char buffer[1024];

        snprintf(buffer, sizeof(buffer), "ENGINE: %s", fmt);
        SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, buffer, args);

        va_end(args);
    }

    void Logger::engine_debug(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);

        char buffer[1024];

        snprintf(buffer, sizeof(buffer), "ENGINE: %s", fmt);
        SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, buffer, args);

        va_end(args);
    }
    
    void Logger::client_info(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);

        char buffer[1024];

        snprintf(buffer, sizeof(buffer), "CLIENT: %s", fmt);
        SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, buffer, args);
        
        va_end(args);
    }

    void Logger::client_warn(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);

        char buffer[1024];

        snprintf(buffer, sizeof(buffer), "CLIENT: %s", fmt);
        SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, buffer, args);

        va_end(args);
    }

    void Logger::client_error(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);

        char buffer[1024];

        snprintf(buffer, sizeof(buffer), "CLIENT: %s", fmt);
        SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, buffer, args);

        va_end(args);
    }
    
    void Logger::client_debug(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);

        char buffer[1024];

        snprintf(buffer, sizeof(buffer), "CLIENT: %s", fmt);
        SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, buffer, args);

        va_end(args);
    }
}
