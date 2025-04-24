#pragma once

#include <utils/tileset.h>

using namespace engine::utils::tileset;
using json = nlohmann::json;

namespace engine {
    class Map {
        public:
            Map(const std::string& workingDir) : m_WorkingDir(workingDir) {}
            ~Map() = default;

            void loadMap(const std::string& fileName, const Vector2& minTileSize, const Vector2& mapOrigin);
            void render();

        private:
            const bool loadTilesets();

            float m_MinTileSize = 0.0f;

            json m_JsonFile;

            Vector2 m_Origin = { 0.0f, 0.0f };
            Vector2 m_TilePosition = { 0.0f, 0.0f };

            Vector2 m_TileSize = { 0.0f, 0.0f };
            Vector2 m_DesiredTileSize = { 0.0f, 0.0f };

            Vector2 m_CropRegion = { 0.0f, 0.0f };
            Vector2 m_CropSize = { 0.0f, 0.0f };

            std::string m_WorkingDir;
            std::string m_FileName;

            std::shared_ptr<Texture> m_Texture;
            std::unordered_map<std::string, Tileset> m_Tilesets;
    };
}
