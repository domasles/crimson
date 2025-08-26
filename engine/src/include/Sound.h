#pragma once

namespace engine {
    class Sound {
        public:
            Sound(const std::string& workingDir) : m_WorkingDir(workingDir), m_Sound(nullptr, Mix_FreeChunk) {}
            ~Sound() = default;

            const bool loadSound(const std::string& fileName);
            const bool play(int loops = 0); // loops >= 0: plays (loops + 1) times; loops = -1: loops forever

            Mix_Chunk* getSound() const;

        private:
            std::string m_WorkingDir;
            std::unique_ptr<Mix_Chunk, void(*)(Mix_Chunk*)> m_Sound;
    };
}
