#include <pch.h>

#include <utils/filesystem.h>
#include <utils/logger.h>

#include <Texture.h>
#include <Core.h>

using namespace engine::utils::filesystem;
using namespace engine::utils::logger;

namespace engine {
    const bool Texture::render(const Vector2& size, const Vector2& position) {
        SDL_FRect destRect{ position.getX(), position.getY(), size.getX(), size.getY() };

        if (!SDL_RenderTexture(Core::getInstance().getRenderer(), getTexture(), nullptr, &destRect)) {
            Logger::engine_error("SDL_RenderTexture Error: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    const bool Texture::render(const Vector2& size, const Vector2& position, const Vector2& cropSize, const Vector2& cropPosition) {
        if (cropSize == Vector2{ 0.0f, 0.0f }) {
            render(size, position);
            return true;
        }

        SDL_FRect destRect{ position.getX(), position.getY(), size.getX(), size.getY() };
        SDL_FRect cropRect{ cropPosition.getRawX(), cropPosition.getRawY(), cropSize.getRawX(), cropSize.getRawY() };

        if (!SDL_RenderTexture(Core::getInstance().getRenderer(), getTexture(), &cropRect, &destRect)) {
            Logger::engine_error("SDL_RenderTexture (cropped) Error: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    SDL_Texture* Texture::getTexture() const {
        if (!m_Texture) {
            Logger::engine_error("Texture is not initialized yet!");
            return nullptr;
        }

        return m_Texture.get();
    }

    const bool Texture::loadTexture(const std::string& fileName, SDL_ScaleMode scaleMode) {
        const std::string& filePath = getGamePath()  + "/" + m_WorkingDir + "/" + fileName;

        SDL_Texture* texture = IMG_LoadTexture(Core::getInstance().getRenderer(), filePath.c_str());

        if (!texture) {
            Logger::engine_error("IMG_LoadTexture Error: {}", SDL_GetError());
            return false;
        }

        SDL_SetTextureScaleMode(texture, scaleMode);
        m_Texture = std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)>(texture, SDL_DestroyTexture);
        std::string relativePath = m_WorkingDir + "/" + fileName;

        if (relativePath.find("assets/") == 0) {
            relativePath = relativePath.substr(7);
        }

        ENGINE_LOG_INIT(("Texture: " + relativePath).c_str());
        return true;
    }
}
