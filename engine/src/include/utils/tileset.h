#pragma once

#include <Texture.h>

namespace engine::utils::tileset {
    struct Tileset {
        std::shared_ptr<Texture> texture = nullptr;

        float textureWidth = 0.0f;
        float textureHeight = 0.0f;

        float tileSize = 0.0f;
        float spacing = 0.0f;
        float padding = 0.0f;
    };
}
