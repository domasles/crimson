#pragma once

#include <SDL3/SDL_log.h>

namespace engine::utils::logger {
    class Logger {
        public:
            static void info(const char* fmt, ...);
            static void warn(const char* fmt, ...);
            static void error(const char* fmt, ...);
            static void debug(const char* fmt, ...);
    };
}
