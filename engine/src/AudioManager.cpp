#include <pch.h>

#include <utils/logger.h>

#include <AudioManager.h>

using namespace engine::utils::logger;

namespace engine {
    AudioManager::AudioManager() = default;
    AudioManager::~AudioManager() = default;

    AudioManager& AudioManager::getInstance() {
        static AudioManager instance;
        return instance;
    }

    bool AudioManager::init() {
        if (m_Initialized) {
            Logger::engine_warn("AudioManager::init() called more than once - ignoring");
            return true;
        }

        if (!MIX_Init()) {
            Logger::engine_error("MIX_Init failed: {}", SDL_GetError());
            return false;
        }

        if (!createMixer()) {
            return false;
        }

        m_Tracks.reserve(32);

        for (int i = 0; i < 32; ++i) {
            MIX_Track* track = MIX_CreateTrack(m_Mixer);
            if (track) {
                m_Tracks.push_back(track);
            }
        }

        m_Initialized = true;
        ENGINE_LOG_INIT("Audio Manager");
        return true;
    }

    void AudioManager::shutdown() {
        if (!m_Initialized) return;

        destroyTracks();

        if (m_Mixer) {
            MIX_DestroyMixer(m_Mixer);
            m_Mixer = nullptr;
        }

        MIX_Quit();

        m_Initialized = false;
        ENGINE_LOG_INIT("Audio Manager shut down");
    }

    bool AudioManager::createMixer() {
        m_Mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

        if (!m_Mixer) {
            Logger::engine_error("MIX_CreateMixerDevice failed: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    void AudioManager::destroyTracks() {
        for (MIX_Track* track : m_Tracks) {
            if (track) {
                MIX_DestroyTrack(track);
            }
        }

        m_Tracks.clear();
    }

    MIX_Track* AudioManager::requestTrack() {
        if (!m_Initialized) return nullptr;

        for (MIX_Track* track : m_Tracks) {
            if (track && !MIX_TrackPlaying(track)) {
                return track;
            }
        }

        MIX_Track* track = MIX_CreateTrack(m_Mixer);
        if (track) {
            m_Tracks.push_back(track);
        }

        return track;
    }

    void AudioManager::releaseTrack(MIX_Track* track) {
        if (!m_Initialized || !track) return;

        MIX_StopTrack(track, 0);
    }
}
