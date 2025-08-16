#include <pch.h>

#include <utils/logger.h>

#include <ResourceManager.h>

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
            Logger::engine_info("Texture cache hit: {}", key);
            return it->second;
        }

        auto texture = std::make_shared<Texture>(workingDir);

        if (texture->loadTexture(fileName)) {
            m_Textures[key] = texture;
            Logger::engine_info("Texture loaded and cached: {}", key);
            return texture;
        }

        Logger::engine_error("Failed to load texture: {}", key);
        return nullptr;
    }

    std::shared_ptr<Sound> ResourceManager::loadSound(const std::string& workingDir, const std::string& fileName) {
        std::string key = makeKey(workingDir, fileName);
        
        auto it = m_Sounds.find(key);

        if (it != m_Sounds.end()) {
            Logger::engine_info("Sound cache hit: {}", key);
            return it->second;
        }

        auto sound = std::make_shared<Sound>(workingDir);

        if (sound->loadSound(fileName)) {
            m_Sounds[key] = sound;
            Logger::engine_info("Sound loaded and cached: {}", key);
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
        Logger::engine_info("Clearing texture cache ({} textures)", m_Textures.size());
        m_Textures.clear();
    }

    void ResourceManager::clearSoundCache() {
        Logger::engine_info("Clearing sound cache ({} sounds)", m_Sounds.size());
        m_Sounds.clear();
    }

    std::string ResourceManager::makeKey(const std::string& workingDir, const std::string& fileName) const {
        std::string path = workingDir + "/" + fileName;

        std::vector<std::string> parts;
        std::stringstream ss(path);
        std::string part;
        
        while (std::getline(ss, part, '/')) {
            if (part == "..") {
                if (!parts.empty() && parts.back() != "..") {
                    parts.pop_back();
                }
            }

            else if (part != "." && !part.empty()) {
                parts.push_back(part);
            }
        }

        std::string normalized;

        for (size_t i = 0; i < parts.size(); ++i) {
            if (i > 0) normalized += "/";
            normalized += parts[i];
        }
        
        return normalized;
    }
}
