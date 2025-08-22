#include <pch.h>

#include <utils/filesystem.h>
#include <utils/logger.h>

#include <Resources.h>

using namespace engine::utils::filesystem;
using namespace engine::utils::logger;

namespace engine {
    ResourceManager& ResourceManager::getInstance() {
        try {
            static ResourceManager instance;
            return instance;
        }

        catch (const std::bad_alloc& e) {
            Logger::engine_error("Memory allocation failed: {}", e.what());
        }

        static ResourceManager fallbackInstance;
        return fallbackInstance;
    }

    std::shared_ptr<Texture> ResourceManager::loadTexture(const std::string& workingDir, const std::string& fileName) {
        std::string key = makeKey(workingDir, fileName);
        auto it = m_Textures.find(key);

        if (it != m_Textures.end()) {
            Logger::engine_debug("Texture cache hit: {}", key);
            return it->second;
        }

        auto texture = std::make_shared<Texture>(workingDir);

        if (texture->loadTexture(fileName)) {
            m_Textures[key] = texture;
            Logger::engine_debug("Texture loaded and cached: {}", key);
            return texture;
        }

        Logger::engine_error("Failed to load texture: {}", key);
        return nullptr;
    }

    std::shared_ptr<Sound> ResourceManager::loadSound(const std::string& workingDir, const std::string& fileName) {
        std::string key = makeKey(workingDir, fileName);
        auto it = m_Sounds.find(key);

        if (it != m_Sounds.end()) {
            Logger::engine_debug("Sound cache hit: {}", key);
            return it->second;
        }

        auto sound = std::make_shared<Sound>(workingDir);

        if (sound->loadSound(fileName)) {
            m_Sounds[key] = sound;
            Logger::engine_debug("Sound loaded and cached: {}", key);
            return sound;
        }

        Logger::engine_error("Failed to load sound: {}", key);
        return nullptr;
    }

    void ResourceManager::clearCache() {
        clearTextureCache();
        clearSoundCache();
    }

    void ResourceManager::clearTextureCache() {
        Logger::engine_debug("Clearing texture cache ({} textures)", m_Textures.size());
        m_Textures.clear();
    }

    void ResourceManager::clearSoundCache() {
        Logger::engine_debug("Clearing sound cache ({} sounds)", m_Sounds.size());
        m_Sounds.clear();
    }

    std::string ResourceManager::makeKey(const std::string& workingDir, const std::string& fileName) const {
        std::string path = workingDir + "/" + fileName;
        return normalizePath(path);
    }
}
