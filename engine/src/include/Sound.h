#pragma once

struct MIX_Audio;

namespace engine {
    class Sound {
        public:
            Sound() = default;
            explicit Sound(const std::string& filePath);
            ~Sound();

            Sound(const Sound&) = delete;
            Sound& operator=(const Sound&) = delete;

            bool loadSound();
            bool isLoaded() const { return m_Audio != nullptr; }

            MIX_Audio* getAudio() const { return m_Audio; }

        private:
            std::string m_FilePath;
            MIX_Audio* m_Audio = nullptr;
    };
}
