#include <pch.h>

#include <utils/filesystem.h>
#include <utils/logger.h>

#include <Sound.h>
#include <Core.h>

using namespace engine::utils::filesystem;
using namespace engine::utils::logger;

namespace engine {
    Sound::Sound(const std::string& filePath) : m_FilePath(filePath) {}

    Sound::~Sound() {
        if (m_Audio) {
            MIX_DestroyAudio(m_Audio);
            m_Audio = nullptr;
        }
    }

    bool Sound::loadSound() {
        const std::string fullPath = getGamePath() + "/" + m_FilePath;
        MIX_Mixer* mixer = Core::getInstance().getMixer();

        if (!mixer) {
            Logger::engine_error("Sound::loadSound() - mixer not initialized");
            return false;
        }

        m_Audio = MIX_LoadAudio(mixer, fullPath.c_str(), false);

        if (!m_Audio) {
            Logger::engine_error("MIX_LoadAudio Error: {}", SDL_GetError());
            return false;
        }

        std::string relativePath = m_FilePath;

        if (relativePath.find("assets/") == 0) {
            relativePath = relativePath.substr(7);
        }

        ENGINE_LOG_INIT(("Sound: " + relativePath).c_str());
        return true;
    }
}
