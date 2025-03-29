#include <pch.h>

#include <utils/filesystem.h>

#include <Texture.h>
#include <Core.h>

using namespace engine::utils::filesystem;

namespace engine {
    const bool Texture::render(const Vector2& size, const Vector2& position) {
        SDL_FRect rect{ position.x, position.y, size.x, size.y };

        if (!SDL_RenderTexture(Core::getInstance().getRenderer(), getTexture(), nullptr, &rect)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_RenderTexture Error: %s", SDL_GetError());
            return false;
        }

        return true;
    }

    SDL_Texture* Texture::getTexture() const {
        if (!m_Texture) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Texture is not initialized yet!");
            return nullptr;
        }

        return m_Texture.get();
    }

    const bool Texture::loadImage(const std::string& fileName, SDL_ScaleMode scaleMode) {
        const std::string& filePath = getGamePath() + fileName;

        SDL_Texture* texture = IMG_LoadTexture(Core::getInstance().getRenderer(), filePath.c_str());

        SDL_SetTextureScaleMode(texture, scaleMode);

        if (!texture) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "IMG_LoadTexture Error: %s", SDL_GetError());
            return false;
        }

        m_Texture = std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)>(texture, SDL_DestroyTexture);
        return true;
    }
}
