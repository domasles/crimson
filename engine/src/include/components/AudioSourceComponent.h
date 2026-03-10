#pragma once

#include <Component.h>
#include <Sound.h>

struct MIX_Track;

namespace engine {
    enum class SoundState { Stopped, Playing, Paused };

    class AudioSourceComponent : public Component {
        public:
            AudioSourceComponent() = default;
            ~AudioSourceComponent() override;

            void setSound(std::shared_ptr<Sound> sound);
            bool hasSound() const { return m_Sound != nullptr; }

            void setLoop(bool loop) { m_Loop = loop; }
            void setVolume(float volume);
            void setAutoPlay(bool autoPlay) { m_AutoPlay = autoPlay; }

            float getVolume() const { return m_Volume; }
            bool  getLoop() const { return m_Loop; }
            bool  isAutoPlay() const { return m_AutoPlay; }

            void play();
            void stop();
            void pause();
            void resume();

            bool isPlaying() const { return m_State == SoundState::Playing; }
            bool isPaused()  const { return m_State == SoundState::Paused; }

            void init() override;
            void onDestroy() override;

            SoundState getState()  const { return m_State; }

        private:
            std::shared_ptr<Sound> m_Sound;
            MIX_Track* m_Track = nullptr;
            SoundState m_State = SoundState::Stopped;

            float m_Volume = 1.0f;
            bool m_Loop = false;
            bool m_AutoPlay = false;
    };
}
