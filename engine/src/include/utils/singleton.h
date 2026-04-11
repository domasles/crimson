#pragma once

#include <utils/logger.h>

using namespace engine::utils::logger;

namespace engine::utils {
    template<typename T>

    T& singleton() {
        static_assert(std::is_default_constructible_v<T>, "Singleton<T> requires a default-constructible type");

        static T* instance = new T(); 
        return *instance;
    }
}
