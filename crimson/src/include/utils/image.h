#pragma once

#include <utils/math.h>

using namespace crimson::utils::math;

namespace crimson::utils::image {
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> loadImage(SDL_Renderer* renderer, const std::string& fileName);
    bool renderTexture(SDL_Renderer* renderer, SDL_Texture* texture, const Vector2 position, const Vector2 stretch);
}
