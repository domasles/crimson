#pragma once

#include <SDL3/SDL_log.h>

#ifdef ENGINE_DEBUG
    #define ENGINE_LOG_INIT(component) Logger::engine_debug("Initialized: {}", component)
#else
    #define ENGINE_LOG_INIT(component) ((void)0)
#endif

namespace engine::utils::logger {
    class Logger {
        public:
            template<typename... Args>
            static void info(std::format_string<Args...> fmt, Args&&... args) {
                auto message = std::format(fmt, std::forward<Args>(args)...);
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
            }

            template<typename... Args>
            static void warn(std::format_string<Args...> fmt, Args&&... args) {
                auto message = std::format(fmt, std::forward<Args>(args)...);
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
            }

            template<typename... Args>
            static void error(std::format_string<Args...> fmt, Args&&... args) {
                auto message = std::format(fmt, std::forward<Args>(args)...);
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
            }

            template<typename... Args>
            static void debug(std::format_string<Args...> fmt, Args&&... args) {
                auto message = std::format(fmt, std::forward<Args>(args)...);
                SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
            }

            template<typename... Args>
            static void engine_info(std::format_string<Args...> fmt, Args&&... args) {
                auto message = std::format("ENGINE: {}", std::format(fmt, std::forward<Args>(args)...));
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
            }

            template<typename... Args>
            static void engine_warn(std::format_string<Args...> fmt, Args&&... args) {
                auto message = std::format("ENGINE: {}", std::format(fmt, std::forward<Args>(args)...));
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
            }

            template<typename... Args>
            static void engine_error(std::format_string<Args...> fmt, Args&&... args) {
                auto message = std::format("ENGINE: {}", std::format(fmt, std::forward<Args>(args)...));
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
            }

            template<typename... Args>
            static void engine_debug(std::format_string<Args...> fmt, Args&&... args) {
                auto message = std::format("ENGINE: {}", std::format(fmt, std::forward<Args>(args)...));
                SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
            }

            template<typename... Args>
            static void client_info(std::format_string<Args...> fmt, Args&&... args) {
                auto message = std::format("CLIENT: {}", std::format(fmt, std::forward<Args>(args)...));
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
            }

            template<typename... Args>
            static void client_warn(std::format_string<Args...> fmt, Args&&... args) {
                auto message = std::format("CLIENT: {}", std::format(fmt, std::forward<Args>(args)...));
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
            }

            template<typename... Args>
            static void client_error(std::format_string<Args...> fmt, Args&&... args) {
                auto message = std::format("CLIENT: {}", std::format(fmt, std::forward<Args>(args)...));
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
            }

            template<typename... Args>
            static void client_debug(std::format_string<Args...> fmt, Args&&... args) {
                auto message = std::format("CLIENT: {}", std::format(fmt, std::forward<Args>(args)...));
                SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
            }
    };
}
