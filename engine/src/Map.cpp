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
            m_Tilesets[fileName].texture->loadImage(fileName);

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
                Logger::engine_error("Map JSON is empty or failed to load: {}", relativePath);
                return false;
            }

            if (!m_JsonFile.contains("__header__")) {
                Logger::engine_error("Missing '__header__' field - not a valid LDtk file: {}", relativePath);
                return false;
            }

            if (!m_JsonFile.contains("levels")) {
                Logger::engine_error("Missing 'levels' field in map: {}", relativePath);
                return false;
            }

            if (!m_JsonFile.contains("defs")) {
                Logger::engine_error("Missing 'defs' field in map: {}", relativePath);
                return false;
            }

            const auto& header = m_JsonFile["__header__"];

            if (!header.contains("fileType") || header["fileType"] != "LDtk Project JSON") {
                Logger::engine_error("Invalid file type - expected 'LDtk Project JSON': {}", relativePath);
                return false;
            }

            const auto& levels = m_JsonFile["levels"];

            if (!levels.is_array() || levels.empty()) {
                Logger::engine_error("Levels field must be a non-empty array: {}", relativePath);
                return false;
            }

            for (size_t i = 0; i < levels.size(); ++i) {
                const auto& level = levels[i];

                if (!level.contains("identifier")) {
                    Logger::engine_error("Level {} missing 'identifier' field: {}", i, relativePath);
                    return false;
                }

                if (!level.contains("pxWid") || !level.contains("pxHei")) {
                    Logger::engine_error("Level '{}' missing size fields: {}", (std::string)level["identifier"], relativePath);
                    return false;
                }

                if (!level.contains("layerInstances")) {
                    Logger::engine_error("Level '{}' missing 'layerInstances' field: {}", (std::string)level["identifier"], relativePath);
                    return false;
                }

                const auto& layerInstances = level["layerInstances"];

                if (!layerInstances.is_array()) {
                    Logger::engine_error("Level '{}' layerInstances must be an array: {}", (std::string)level["identifier"], relativePath);
                    return false;
                }

                for (size_t j = 0; j < layerInstances.size(); ++j) {
                    const auto& layer = layerInstances[j];

                    if (!layer.contains("__identifier") || !layer.contains("__type")) {
                        Logger::engine_error("Layer {} in level '{}' missing required fields: {}", j, (std::string)level["identifier"], relativePath);
                        return false;
                    }

                    const std::string layerType = layer["__type"];

                    if (layerType != "Tiles" && layerType != "Entities" && layerType != "IntGrid" && layerType != "AutoLayer") {
                        Logger::engine_error("Invalid layer type '{}' in level '{}': {}", layerType, (std::string)level["identifier"], relativePath);
                        return false;
                    }

                    if (layerType == "Entities") {
                        if (!layer.contains("entityInstances")) {
                            Logger::engine_error("Entity layer '{}' missing 'entityInstances' field: {}", (std::string)layer["__identifier"], relativePath);
                            return false;
                        }

                        const auto& entities = layer["entityInstances"];

                        if (!entities.is_array()) {
                            Logger::engine_error("Entity layer '{}' entityInstances must be an array: {}", (std::string)layer["__identifier"], relativePath);
                            return false;
                        }

                        for (size_t k = 0; k < entities.size(); ++k) {
                            const auto& entity = entities[k];

                            if (!entity.contains("__identifier") || !entity.contains("px")) {
                                Logger::engine_error("Entity {} in layer '{}' missing required fields: {}", k, (std::string)layer["__identifier"], relativePath);
                                return false;
                            }

                            const auto& px = entity["px"];

                            if (!px.is_array() || px.size() != 2) {
                                Logger::engine_error("Entity '{}' has invalid position format: {}", (std::string)entity["__identifier"], relativePath);
                                return false;
                            }
                        }
                    }

                    if (layerType == "Tiles") {
                        if (!layer.contains("gridTiles")) {
                            Logger::engine_error("Tile layer '{}' missing 'gridTiles' field: {}", (std::string)layer["__identifier"], relativePath);
                            return false;
                        }

                        if (!layer.contains("__gridSize")) {
                            Logger::engine_error("Tile layer '{}' missing '__gridSize' field: {}", (std::string)layer["__identifier"], relativePath);
                            return false;
                        }
                    }
                }
            }

            const auto& defs = m_JsonFile["defs"];

            if (!defs.contains("tilesets")) {
                Logger::engine_error("Missing 'tilesets' in definitions: {}", relativePath);
                return false;
            }

            const auto& tilesets = defs["tilesets"];

            if (!tilesets.is_array()) {
                Logger::engine_error("Tilesets must be an array: {}", relativePath);
                return false;
            }

            for (size_t i = 0; i < tilesets.size(); ++i) {
                const auto& tileset = tilesets[i];

                if (!tileset.contains("identifier") || !tileset.contains("relPath")) {
                    Logger::engine_error("Tileset {} missing required fields: {}", i, relativePath);
                    return false;
                }

                if (!tileset.contains("pxWid") || !tileset.contains("pxHei") || !tileset.contains("tileGridSize")) {
                    Logger::engine_error("Tileset '{}' missing size fields: {}", (std::string)tileset["identifier"], relativePath);
                    return false;
                }

                const std::string& fullRelPath = m_WorkingDir + "/" + (std::string)tileset["relPath"];
                const std::string& filePath = std::string(SDL_GetBasePath()) + "games/" + Core::getInstance().getName() + "/" + fullRelPath;
                
                std::ifstream tilesetFile(filePath);

                if (!tilesetFile.good()) {
                    Logger::engine_error("Tileset file not found: {} (map: {})", filePath, relativePath);
                    return false;
                }

                tilesetFile.close();
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
