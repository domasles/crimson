#pragma once

#ifdef ENGINE_PLATFORM_WINDOWS
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #define NODRAWTEXT
    #define NOUSER
    #define NOMSG

    #include <windows.h>
#elif defined(ENGINE_PLATFORM_MACOS)
    #include <TargetConditionals.h>
#endif

#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL.h>

#include <rmlui/Core/ElementDocument.h>
#include <rmlui/Core/RenderInterface.h>
#include <rmlui/Core/SystemInterface.h>
#include <rmlui/Core/Context.h>
#include <rmlui/Core/Input.h>
#include <rmlui/Core.h>

#include <nlohmann/json.hpp>

#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <thread>
#include <memory>
#include <mutex>

#include <concepts>
#include <cstdarg>
#include <fstream>
#include <format>
#include <string>

#include <algorithm>
#include <random>
#include <vector>
#include <cmath>

#include <functional>
#include <cstdint>
#include <chrono>
#include <ranges>
#include <array>
#include <deque>
#include <queue>
