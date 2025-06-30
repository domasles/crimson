#include <pch.h>

#include <utils/filesystem.h>
#include <utils/logger.h>

#include <Core.h>
#include <Map.h>

using namespace engine::utils::filesystem;
using namespace engine::utils::logger;

namespace engine {
    void Map::loadMap(const std::string& fileName, const Vector2& minTileSize, const Vector2& mapOrigin) {
        m_FileName = fileName;
        m_DesiredTileSize = minTileSize;
        m_Origin = mapOrigin;

        const std::string& filePath = std::string(SDL_GetBasePath()) + "games/" + Core::getInstance().getName() + "/" + m_WorkingDir + "/" + m_FileName;

        loadJSONFile(filePath, &m_JsonFile);

        if (!validateMapStructure()) {
            std::string relativePath = m_WorkingDir + "/" + m_FileName;

            if (relativePath.find("assets/") == 0) {
                relativePath = relativePath.substr(7);
            }

            Logger::engine_error("Map validation failed for: {}", relativePath);
            return;
        }
        
        loadTilesets();
        loadTiles();
    }

    void Map::loadTiles() {
        textureQueue = std::make_shared<TileRenderQueue>();
        textureQueue->setDirection(Reversed);

        for (const auto& level : m_JsonFile["levels"]) {
            for (const auto& layer : level["layerInstances"]) {
                if (!layer["__tilesetRelPath"].is_string()) {
                    continue;
                }

                const std::string& tilesetRelPath = layer["__tilesetRelPath"];
                const std::string& tilesetKey = getFileName(tilesetRelPath);

                const auto& tileset = m_Tilesets[tilesetKey];

                for (const auto& tile : layer["gridTiles"]) {
                    const float posX = tile["px"][0];
                    const float posY = tile["px"][1];

                    const float cropX = tile["src"][0];
                    const float cropY = tile["src"][1];

                    m_TilePosition = { posX, posY };
                    m_TilePosition *= m_DesiredTileSize / m_MinTileSize;

                    m_TileSize = m_DesiredTileSize / (m_MinTileSize / tileset.tileSize);

                    m_CropRegion = { cropX, cropY };
                    m_CropSize = { tileset.tileSize, tileset.tileSize };

                    textureQueue->add(tileset.texture, m_TileSize, m_TilePosition + m_Origin, m_CropSize, m_CropRegion);
                }
            }
        }

        std::string relativePath = m_WorkingDir + "/" + m_FileName;

        if (relativePath.find("assets/") == 0) {
            relativePath = relativePath.substr(7);
        }
        
        ENGINE_LOG_INIT(("Map: " + relativePath).c_str());
    }

    void Map::render() {
        RenderQueueManager::getInstance().render(textureQueue);
    }

    const bool Map::loadTilesets() {
        for (const auto& tileset : m_JsonFile["defs"]["tilesets"]) {
            const std::string& fullRelPath = m_WorkingDir + "/" + (std::string)tileset["relPath"];

            const std::string& relPath = getParentPath(fullRelPath);
            const std::string& fileName = getFileName(fullRelPath);

            const float pxWid = tileset["pxWid"];
            const float pxHei = tileset["pxHei"];

            const float tileGridSize = tileset["tileGridSize"];

            const float spacing = tileset["spacing"];
            const float padding = tileset["padding"];

            std::shared_ptr<Texture> tilesetTexture = std::make_shared<Texture>(relPath);

            m_Tilesets[fileName] = Tileset{};

            m_Tilesets[fileName].texture = std::move(tilesetTexture);
            m_Tilesets[fileName].texture->loadTexture(fileName);

            m_Tilesets[fileName].textureSize.set(pxWid, pxHei);
            m_Tilesets[fileName].tileSize = tileGridSize;

            m_Tilesets[fileName].spacing = spacing;
            m_Tilesets[fileName].padding = padding;

            if (m_MinTileSize == 0.0f || tileGridSize < m_MinTileSize) {
                m_MinTileSize = tileGridSize;
            }
        }
        
        return true;
    }

    std::vector<std::pair<std::string, Vector2>> Map::getEntitiesPositions() const {
        std::vector<std::pair<std::string, Vector2>> entities;

        for (const auto& level : m_JsonFile["levels"]) {
            for (const auto& layer : level["layerInstances"]) {
                if (layer["__type"] != "Entities") {
                    continue;
                }

                for (const auto& entity : layer["entityInstances"]) {
                    const std::string& entityName = entity["__identifier"];

                    const float posX = entity["px"][0];
                    const float posY = entity["px"][1];

                    Vector2 entityPosition = { posX, posY };

                    entityPosition *= m_DesiredTileSize / m_MinTileSize;
                    entityPosition += m_Origin;
                    
                    entities.emplace_back(entityName, entityPosition);
                }
            }
        }
        
        return entities;
    }
    
    const bool Map::validateMapStructure() const {
        std::string relativePath = m_WorkingDir + "/" + m_FileName;
        if (relativePath.find("assets/") == 0) {
            relativePath = relativePath.substr(7);
        }
        
        try {
            if (m_JsonFile.empty()) {
                Logger::engine_error("Map JSON failed to load: {}", relativePath);
                return false;
            }

            if (!m_JsonFile.contains("__header__") || !m_JsonFile["__header__"].contains("fileType") || m_JsonFile["__header__"]["fileType"] != "LDtk Project JSON") {
                Logger::engine_error("Not a valid LDtk file: {}", relativePath);
                return false;
            }

            if (!m_JsonFile.contains("levels") || !m_JsonFile["levels"].is_array()) {
                Logger::engine_error("Missing or invalid levels array: {}", relativePath);
                return false;
            }

            if (!m_JsonFile.contains("defs") || !m_JsonFile["defs"].contains("tilesets") || !m_JsonFile["defs"]["tilesets"].is_array()) {
                Logger::engine_error("Missing tileset definitions: {}", relativePath);
                return false;
            }

            for (const auto& tileset : m_JsonFile["defs"]["tilesets"]) {
                if (!tileset.contains("relPath") || !tileset.contains("pxWid") || !tileset.contains("pxHei") || !tileset.contains("tileGridSize")) {
                    Logger::engine_error("Tileset missing required fields: {}", relativePath);
                    return false;
                }
            }

            Logger::engine_info("Map validation passed for: {}", relativePath);
            return true;
        }

        catch (const std::exception& e) {
            Logger::engine_error("Map validation error: {} (map: {})", e.what(), relativePath);
            return false;
        }
    }
}
