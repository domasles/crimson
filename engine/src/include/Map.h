#pragma once

#include <utils/rendering.h>
#include <utils/tileset.h>

#include <Texture.h>
#include <Core.h>

using namespace engine::utils::rendering;
using namespace engine::utils::tileset;

using json = nlohmann::json;

namespace engine {
    class Map {
        public:
            Map(const std::string& workingDir) : m_WorkingDir(workingDir) {}
            ~Map() = default;
            
            void loadMap(const std::string& fileName, const Vector2& minTileSize, const Vector2& mapOrigin);
            void loadTiles();
            void loadCollisionData();
            void render();

            Vector2 getEntityPosition(std::string entityName) const;
            std::vector<std::pair<std::string, Vector2>> getEntitiesPositions() const;
            
            // Collision data structures
            struct CollisionTile {
                Vector2 worldPosition;
                Vector2 size;
                int collisionValue;
                std::string layerIdentifier;
            };
            
            std::vector<CollisionTile> getCollisionTiles() const { return m_CollisionTiles; }
            bool hasCollisionData() const { return !m_CollisionTiles.empty(); }
            bool checkCollisionAt(const Vector2& worldPos, const Vector2& size) const;
        
        private:
            const bool loadTilesets();
            const bool validateMapStructure() const;

            float m_MinTileSize = 0.0f;

            json m_JsonFile;

            Core& m_EngineCore = Core::getInstance();

            Vector2 m_Origin{ 0.0f, 0.0f };
            Vector2 m_TilePosition{ 0.0f, 0.0f };

            Vector2 m_TileSize{ 0.0f, 0.0f };
            Vector2 m_DesiredTileSize{ 0.0f, 0.0f };

            Vector2 m_CropRegion{ 0.0f, 0.0f };
            Vector2 m_CropSize{ 0.0f, 0.0f };

            std::string m_WorkingDir;
            std::string m_FileName;

            std::shared_ptr<TileRenderQueue> textureQueue;
            std::unordered_map<std::string, Tileset> m_Tilesets;
            std::vector<CollisionTile> m_CollisionTiles;
            
            void parseIntGridLayers();
    };
}
