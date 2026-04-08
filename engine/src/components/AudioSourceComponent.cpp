#include <pch.h>

#include <utils/logger.h>

#include <components/AudioSourceComponent.h>
#include <Core.h>

using namespace engine::utils::logger;

namespace engine {
    AudioSourceComponent::~AudioSourceComponent() {
        onDestroy();
    }

    void AudioSourceComponent::setSound(std::shared_ptr<Sound> sound) {
        if (m_Track) {
            MIX_StopTrack(m_Track, 0);
            MIX_DestroyTrack(m_Track);

            m_Track = nullptr;
        }

        m_Sound = std::move(sound);
        m_State = SoundState::Stopped;

        if (m_Sound && m_Sound->isLoaded()) {
            MIX_Mixer* mixer = Core::getInstance().getMixer();
            if (mixer) m_Track = MIX_CreateTrack(mixer);
        }

        if (m_Track) MIX_SetTrackGain(m_Track, m_Volume);
    }

    void AudioSourceComponent::setVolume(float volume) {
        m_Volume = std::clamp(volume, 0.0f, 1.0f);
        if (m_Track) MIX_SetTrackGain(m_Track, m_Volume);
    }

    void AudioSourceComponent::play() {
        if (!m_Track || !m_Sound || !m_Sound->getAudio()) {
            Logger::engine_error("AudioSourceComponent::play() - no valid sound");
            return;
        }

        MIX_Track* trackToUse = m_Track;

        if (MIX_TrackPlaying(trackToUse) && !m_Loop) {
            MIX_Track* freeTrack = Core::getInstance().getFreeTrack();

            if (freeTrack) {
                trackToUse = freeTrack;
            }
        }

        MIX_SetTrackAudio(trackToUse, m_Sound->getAudio());
        MIX_SetTrackGain(trackToUse, m_Volume);

        SDL_PropertiesID props = SDL_CreateProperties();
        SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, m_Loop ? -1 : 0);

        if (!MIX_PlayTrack(trackToUse, props)) {
            SDL_DestroyProperties(props);
            Logger::engine_error("MIX_PlayTrack Error: {}", SDL_GetError());
            return;
        }

        SDL_DestroyProperties(props);
        if (trackToUse == m_Track) {
            m_State = SoundState::Playing;
        }
    }

    void AudioSourceComponent::stop() {
        if (m_Track) MIX_StopTrack(m_Track, 0);
        m_State = SoundState::Stopped;
    }

    void AudioSourceComponent::pause() {
        if (m_Track && m_State == SoundState::Playing) {
            MIX_PauseTrack(m_Track);
            m_State = SoundState::Paused;
        }
    }

    void AudioSourceComponent::resume() {
        if (m_Track && m_State == SoundState::Paused) {
            MIX_ResumeTrack(m_Track);
            m_State = SoundState::Playing;
        }
    }

    void AudioSourceComponent::init() {
        if (m_AutoPlay) play();
    }

    void AudioSourceComponent::onDestroy() {
        if (m_Track) {
            MIX_StopTrack(m_Track, 0);
            MIX_DestroyTrack(m_Track);

            m_Track = nullptr;
        }

        m_State = SoundState::Stopped;
    }
}
