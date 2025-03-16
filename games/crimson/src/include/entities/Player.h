#pragma once

#include <Entity.h>

using namespace engine;

namespace crimson {
    class Player : public Entity {
        public:
            Player() : m_Texture(nullptr, SDL_DestroyTexture) {}

            void init() override;
            void update(float deltaTime) override;
            void render() override;

        private:
            float m_Speed = 200;
            std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> m_Texture;
    };
}
