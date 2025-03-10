#pragma once

#include <entities/Entity.h>

using namespace crimson::utils::image;

namespace crimson {
    class Player : public Entity {
        public:
            Player() : m_Texture(nullptr, SDL_DestroyTexture) {}

            void init() override;
            void update(float deltatime) override {}
            void render() override;

        private:
            std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> m_Texture;
    };
}
