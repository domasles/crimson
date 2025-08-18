#pragma once

#ifdef ENGINE_PLATFORM_WINDOWS
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#elif defined(ENGINE_PLATFORM_MACOS)
    #include <TargetConditionals.h>
#endif

#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL.h>

#include <nlohmann/json.hpp>

#include <unordered_map>
#include <typeindex>
#include <thread>
#include <memory>
#include <mutex>

#include <concepts>
#include <iostream>
#include <cstdarg>
#include <fstream>
#include <format>
#include <string>

#include <vector>
#include <cmath>

#include <functional>
#include <chrono>
#include <ranges>
#include <deque>
#include <queue>
