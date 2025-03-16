#pragma once

#include <utils/math.h>

using namespace engine::utils::math;

namespace engine::utils::image {
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> loadImage(SDL_Renderer* renderer, const std::string& fileName);
    bool renderTexture(SDL_Renderer* renderer, SDL_Texture* texture, const Vector2 position, const Vector2 stretch);
}
