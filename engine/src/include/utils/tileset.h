#pragma once

#include <Texture.h>

namespace engine::utils::tileset {
    struct Tileset {
        std::shared_ptr<Texture> texture = nullptr;

        float tileSize = 0.0f;
        float spacing = 0.0f;
        float padding = 0.0f;

        Vector2 textureSize{ 0.0f, 0.0f };
    };
}
