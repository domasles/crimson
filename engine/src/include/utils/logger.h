#pragma once

#include <SDL3/SDL_log.h>

#ifdef ENGINE_DEBUG
    #define ENGINE_LOG_INIT(component) Logger::engine_debug("Initialized: %s", component)
#else
    #define ENGINE_LOG_INIT(component) ((void)0)
#endif

namespace engine::utils::logger {
    class Logger {
        public:
            static void info(const char* fmt, ...);
            static void warn(const char* fmt, ...);
            static void error(const char* fmt, ...);
            static void debug(const char* fmt, ...);

            static void client_info(const char* fmt, ...);
            static void client_warn(const char* fmt, ...);
            static void client_error(const char* fmt, ...);
            static void client_debug(const char* fmt, ...);

            static void engine_info(const char* fmt, ...);
            static void engine_warn(const char* fmt, ...);
            static void engine_error(const char* fmt, ...);
            static void engine_debug(const char* fmt, ...);
    };
}
