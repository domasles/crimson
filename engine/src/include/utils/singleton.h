#pragma once

#include <utils/logger.h>

using namespace engine::utils::logger;

namespace engine::utils {
    template<typename T>

    T& singleton() {
        static_assert(std::is_default_constructible_v<T>, "Singleton<T> requires a default-constructible type");

        try {
            static T& instance = *new T();
            return instance;
        }

        catch (const std::bad_alloc& e) {
            Logger::engine_error("Memory allocation failed: {}", e.what());
        }

        static T& fallbackInstance = *new T();
        return fallbackInstance;
    }
}
