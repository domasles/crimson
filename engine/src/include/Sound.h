#pragma once

namespace engine {
    class Sound {
        public:
            Sound(const std::string& workingDir) : m_WorkingDir(workingDir), m_Audio(nullptr) {}
            ~Sound();

            bool loadSound(const std::string& fileName);
            bool play(int loops = 0); // loops >= 0: plays (loops + 1) times; loops = -1: loops forever

            MIX_Audio* getAudio() const { return m_Audio; }

        private:
            std::string m_WorkingDir;
            MIX_Audio* m_Audio;
    };
}
