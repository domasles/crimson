#include <pch.h>

#include <utils/filesystem.h>

#include <Core.h>
#include <Map.h>

using namespace engine::utils::filesystem;

namespace engine {
    void Map::loadMap(const std::string& fileName, const Vector2& minTileSize, const Vector2& mapOrigin) {
        m_FileName = fileName;
        m_DesiredTileSize = minTileSize;
        m_Origin = mapOrigin;

        const std::string& filePath = std::string(SDL_GetBasePath()) + "games/" + Core::getInstance().getName() + "/" + m_WorkingDir + "/" + m_FileName;

        loadJSONFile(filePath, &m_JsonFile);
        loadTilesets();
        loadTiles();
    }

    void Map::loadTiles() {
        const std::string& filePath = std::string(SDL_GetBasePath()) + "games/" + Core::getInstance().getName() + "/" + m_WorkingDir + "/" + m_FileName;
        std::ifstream file(filePath);

        if (!file.is_open()) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open file: %s", filePath.c_str());
            return;
        }

        try {
            if (!m_JsonFile.contains("levels")) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Missing 'levels' field in JSON.");
                return;
            }

            textureQueue = std::make_shared<TileRenderQueue>();
            textureQueue->setDirection(Reversed);

            for (const auto& level : m_JsonFile["levels"]) {
                if (!level.contains("layerInstances")) {
                    std::string id = level["identifier"];

                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Missing 'layerInstances' field in level: %s.", id.c_str());

                    continue;
                }

                for (const auto& layer : level["layerInstances"]) {
                    if (!layer["__tilesetRelPath"].is_string()) {
                        continue;
                    }

                    const std::string& tilesetRelPath = layer["__tilesetRelPath"];
                    const std::string& tilesetKey = getFileName(tilesetRelPath);

                    if (m_Tilesets.find(tilesetKey) == m_Tilesets.end()) {
                        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Tileset not found: %s", tilesetKey.c_str());
                        continue;
                    }

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
        }

        catch (const std::exception& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSON parsing error: %s", e.what());
        }
    }

    void Map::render() {
        RenderQueueManager::getInstance().render(textureQueue);
    }

    const bool Map::loadTilesets() {
        try {
            if (!m_JsonFile.contains("__header__")) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Missing '__header__' field in JSON.");
                return false;
            }

            if (!m_JsonFile.contains("defs")) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Missing 'defs' field in JSON.");
                return false;
            }

            if (!m_JsonFile["defs"].contains("tilesets")) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No 'tilesets' definition found.");
                return false;
            }

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

                m_Tilesets[fileName].textureWidth = pxWid;
                m_Tilesets[fileName].textureHeight = pxHei;

                m_Tilesets[fileName].tileSize = tileGridSize;

                m_Tilesets[fileName].spacing = spacing;
                m_Tilesets[fileName].padding = padding;

                if (m_MinTileSize == 0.0f || tileGridSize < m_MinTileSize) {
                    m_MinTileSize = tileGridSize;
                }
            }
        }

        catch (const std::exception& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "JSON parsing error: %s", e.what());
        }

        return false;
    }
}
