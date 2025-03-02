#pragma once

#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL.h>

#include <memory>
#include <string>

namespace crimson {
    class Image {
        public:
            static std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> LoadImage(SDL_Renderer* renderer, const std::string& fileName);
            static bool RenderTexture(SDL_Renderer* renderer, SDL_Texture* texture, const float posX, const float posY, const float stretchX, const float stretchY);
    };
}
