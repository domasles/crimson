#include <utils/image.h>

namespace crimson::utils::image {
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> LoadImage(SDL_Renderer* renderer, const std::string& fileName) {
        std::string basePath = SDL_GetBasePath();
        std::string fullPath = basePath + fileName;

        SDL_Texture* texture = IMG_LoadTexture(renderer, fullPath.c_str());

        if (!texture) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "IMG_LoadTexture Error: %s", SDL_GetError());
            return std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)>(nullptr, SDL_DestroyTexture);
        }

        return std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)>(texture, SDL_DestroyTexture);
    }

    bool RenderTexture(SDL_Renderer* renderer, SDL_Texture* texture, const float posX, const float posY, const float stretchX, const float stretchY) {
        SDL_FRect rect{ posX, posY, stretchX, stretchY };
        
        if (!SDL_RenderTexture(renderer, texture, nullptr, &rect)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_RenderTexture Error: %s", SDL_GetError());
            return false;
        }

        return true;
    }
}
