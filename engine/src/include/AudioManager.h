#pragma once

#include <vector>

struct MIX_Mixer;
struct MIX_Track;

namespace engine {
    class AudioManager {
        public:
            static AudioManager& getInstance();

            AudioManager() = default;
            ~AudioManager() = default;

            bool init();
            void shutdown();

            MIX_Mixer* getMixer() const { return m_Mixer; }
            MIX_Track* requestTrack();

            void releaseTrack(MIX_Track* track);
            bool isInitialized() const { return m_Initialized; }

        private:
            AudioManager(const AudioManager&) = delete;
            AudioManager& operator=(const AudioManager&) = delete;

            bool createMixer();
            void destroyTracks();

            bool m_Initialized = false;

            MIX_Mixer* m_Mixer = nullptr;
            std::vector<MIX_Track*> m_Tracks;
    };
}
