#pragma once

#include <utils/math.h>

using namespace engine::utils::math;

namespace engine {
    class Texture {
        public:
            Texture(const std::string& workingDir) : m_Texture(nullptr, SDL_DestroyTexture), m_WorkingDir(workingDir) {}

            const bool loadImage(const std::string& fileName, SDL_ScaleMode scaleMode=SDL_SCALEMODE_NEAREST);

            const bool render(const Vector2& size, const Vector2& position);
            const bool render(const Vector2& size, const Vector2& position, const Vector2& cropSize, const Vector2& cropPosition);

            SDL_Texture* getTexture() const;

        private:
            std::string m_WorkingDir;
            std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> m_Texture;
    };
}
