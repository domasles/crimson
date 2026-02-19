#include <pch.h>

#include <utils/filesystem.h>
#include <utils/logger.h>

#include <Sound.h>
#include <Core.h>

using namespace engine::utils::filesystem;
using namespace engine::utils::logger;

namespace engine {
    Sound::~Sound() {
        if (m_Audio) {
            MIX_DestroyAudio(m_Audio);
            m_Audio = nullptr;
        }
    }

    bool Sound::loadSound(const std::string& fileName) {
        std::string filePath = getGamePath() + "/" + m_WorkingDir + "/" + fileName;
        MIX_Mixer* mixer = Core::getInstance().getMixer();

        if (!mixer) {
            Logger::engine_error("Mixer not initialized");
            return false;
        }

        m_Audio = MIX_LoadAudio(mixer, filePath.c_str(), false);

        if (!m_Audio) {
            Logger::engine_error("MIX_LoadAudio Error: {}", SDL_GetError());
            return false;
        }

        std::string relativePath = m_WorkingDir + "/" + fileName;

        if (relativePath.find("assets/") == 0) {
            relativePath = relativePath.substr(7);
        }

        ENGINE_LOG_INIT(("Sound: " + relativePath).c_str());
        return true;
    }

    bool Sound::play(int loops) {
        if (!m_Audio) {
            Logger::engine_error("Sound not loaded!");
            return false;
        }

        MIX_Track* track = Core::getInstance().getFreeTrack();

        if (!track) {
            Logger::engine_error("No available track to play sound");
            return false;
        }

        MIX_SetTrackAudio(track, m_Audio);
        SDL_PropertiesID props = SDL_CreateProperties();
        SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loops);

        if (!MIX_PlayTrack(track, props)) {
            SDL_DestroyProperties(props);
            Logger::engine_error("MIX_PlayTrack Error: {}", SDL_GetError());
            return false;
        }

        SDL_DestroyProperties(props);
        return true;
    }
}
