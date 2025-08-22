#pragma once

#include <Component.h>
#include <Texture.h>

#include <memory>

namespace engine {
    class TextureComponent : public Component {
        public:
            TextureComponent() = default;
            virtual ~TextureComponent() = default;

            void setTexture(std::shared_ptr<Texture> texture) { m_Texture = texture; }
            bool hasTexture() const { return m_Texture != nullptr; }

            std::shared_ptr<Texture> getTexture() const { return m_Texture; }

            void render();
            void render(const SDL_FRect& cropRegion);
            void render(const SDL_FRect& cropRegion, const SDL_FRect& destinationRegion);

        private:
            std::shared_ptr<Texture> m_Texture = nullptr;
    };
}
