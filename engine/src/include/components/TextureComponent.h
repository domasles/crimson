#pragma once

#include <utils/math.h>

#include <Component.h>
#include <Texture.h>

using namespace engine::utils::math;

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
            
            void renderGizmo(SDL_Renderer* renderer, const Vector2& position, const Vector2& size) const;

        private:
            std::shared_ptr<Texture> m_Texture = nullptr;
    };
}
