#pragma once

#include <utils/math.h>

using namespace engine::utils::math;

namespace engine {
    class Texture {
        public:
            Texture(const std::string& fileName) : m_Texture(nullptr, SDL_DestroyTexture) { loadImage(fileName); }

            const bool render(const Vector2& size, const Vector2& position);
            SDL_Texture* getTexture() const;

        private:
            const bool loadImage(const std::string& fileName, SDL_ScaleMode scaleMode = SDL_SCALEMODE_NEAREST);
            std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> m_Texture;
    };
}
