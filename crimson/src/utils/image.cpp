#include <pch.h>

#include <utils/image.h>

namespace crimson::utils::image {
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> loadImage(SDL_Renderer* renderer, const std::string& fileName) {
        std::string basePath = SDL_GetBasePath();
        std::string fullPath = basePath + fileName;

        SDL_Texture* texture = IMG_LoadTexture(renderer, fullPath.c_str());

        if (!texture) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "IMG_LoadTexture Error: %s", SDL_GetError());
            return std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)>(nullptr, SDL_DestroyTexture);
        }

        return std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)>(texture, SDL_DestroyTexture);
    }

    bool renderTexture(SDL_Renderer* renderer, SDL_Texture* texture, const Vector2 position, const Vector2 stretch) {
        SDL_FRect rect{ position.x, position.y, stretch.x, stretch.y };
        
        if (!SDL_RenderTexture(renderer, texture, nullptr, &rect)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_RenderTexture Error: %s", SDL_GetError());
            return false;
        }

        return true;
    }
}
