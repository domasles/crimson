#include <pch.h>

#include <utils/filesystem.h>
#include <utils/logger.h>

#include <Texture.h>
#include <Core.h>

using namespace engine::utils::filesystem;
using namespace engine::utils::logger;

namespace engine {
    Texture::~Texture() {
        if (m_TextureID != 0) {
            glDeleteTextures(1, &m_TextureID);
            m_TextureID = 0;
        }
    }

    bool Texture::render(const Vector2& size, const Vector2& position, float rotation, const Vector2& scale) {
        if (m_TextureID == 0) {
            Logger::engine_error("Texture not loaded");
            return false;
        }

        Core::getInstance().getRenderer()->drawQuad(position, size, m_TextureID, Color(1.0f, 1.0f, 1.0f, 1.0f), rotation, scale);
        return true;
    }

    bool Texture::render(const Vector2& size, const Vector2& position, const Vector2& cropSize, const Vector2& cropPosition, float rotation, const Vector2& scale) {
        if (cropSize == Vector2{ 0.0f, 0.0f }) {
            return render(size, position, rotation, scale);
        }

        if (m_TextureID == 0) {
            Logger::engine_error("Texture not loaded");
            return false;
        }

        float u0 = cropPosition.getRawX() / static_cast<float>(m_Width);
        float v0 = cropPosition.getRawY() / static_cast<float>(m_Height);
        float u1 = (cropPosition.getRawX() + cropSize.getRawX()) / static_cast<float>(m_Width);
        float v1 = (cropPosition.getRawY() + cropSize.getRawY()) / static_cast<float>(m_Height);

        Core::getInstance().getRenderer()->drawQuad(position, size, Vector2{u0, v0}, Vector2{u1, v1}, m_TextureID, Color(1.0f, 1.0f, 1.0f, 1.0f), rotation, scale);
        return true;
    }

    bool Texture::loadTexture(const std::string& fileName, bool linearFiltering) {
        const std::string& filePath = getGamePath() + "/" + m_WorkingDir + "/" + fileName;

        SDL_Surface* surface = IMG_Load(filePath.c_str());

        if (!surface) {
            Logger::engine_error("IMG_Load Error: {}", SDL_GetError());
            return false;
        }

        m_Width = surface->w;
        m_Height = surface->h;

        // Convert surface to RGBA format if needed
        SDL_Surface* rgbaSurface = nullptr;
        if (surface->format != SDL_PIXELFORMAT_RGBA32) {
            rgbaSurface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
            SDL_DestroySurface(surface);

            if (!rgbaSurface) {
                Logger::engine_error("SDL_ConvertSurface Error: {}", SDL_GetError());
                return false;
            }
            surface = rgbaSurface;
        }

        // Generate OpenGL texture
        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);

        // Set texture parameters
        GLint filterMode = linearFiltering ? GL_LINEAR : GL_NEAREST;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Upload texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

        SDL_DestroySurface(surface);
        glBindTexture(GL_TEXTURE_2D, 0);

        std::string relativePath = m_WorkingDir + "/" + fileName;

        if (relativePath.find("assets/") == 0) {
            relativePath = relativePath.substr(7);
        }

        ENGINE_LOG_INIT(("Texture: " + relativePath).c_str());
        return true;
    }
}
