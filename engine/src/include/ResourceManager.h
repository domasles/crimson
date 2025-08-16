#pragma once

#include <Texture.h>
#include <Sound.h>

namespace engine {
    class ResourceManager {
        public:
            static ResourceManager& getInstance();

            std::shared_ptr<Texture> loadTexture(const std::string& workingDir, const std::string& fileName);
            std::shared_ptr<Sound> loadSound(const std::string& workingDir, const std::string& fileName);

            void clearCache();
            void clearTextureCache();
            void clearSoundCache();

            size_t getTextureCount() const { return m_Textures.size(); }
            size_t getSoundCount() const { return m_Sounds.size(); }

        private:
            ResourceManager() = default;
            ~ResourceManager() = default;

            ResourceManager(const ResourceManager&) = delete;
            ResourceManager& operator=(const ResourceManager&) = delete;

            std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
            std::unordered_map<std::string, std::shared_ptr<Sound>> m_Sounds;

            std::string makeKey(const std::string& workingDir, const std::string& fileName) const;
    };

    inline ResourceManager& getResources() { 
        return ResourceManager::getInstance(); 
    }
    
    inline std::shared_ptr<Texture> loadTexture(const std::string& workingDir, const std::string& fileName) {
        return getResources().loadTexture(workingDir, fileName);
    }
    
    inline std::shared_ptr<Sound> loadSound(const std::string& workingDir, const std::string& fileName) {
        return getResources().loadSound(workingDir, fileName);
    }
}
