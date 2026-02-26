#pragma once

namespace engine {
    class Sound {
        public:
            Sound(const std::string& filePath) : m_FilePath(filePath), m_Audio(nullptr) {}
            ~Sound();

            bool loadSound();
            bool play(int loops = 0); // loops >= 0: plays (loops + 1) times; loops = -1: loops forever

            MIX_Audio* getAudio() const { return m_Audio; }

        private:
            std::string m_FilePath;
            MIX_Audio* m_Audio;
    };
}
