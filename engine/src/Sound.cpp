#include <pch.h>

#include <utils/filesystem.h>
#include <utils/logger.h>

#include <Sound.h>
#include <Core.h>

using namespace engine::utils::filesystem;
using namespace engine::utils::logger;

namespace engine {
    const bool Sound::loadSound(const std::string& fileName) {
        std::string filePath = getGamePath() + "/" + m_WorkingDir + "/" + fileName;
        Mix_Chunk* chunk = Mix_LoadWAV(filePath.c_str());

        if (!chunk) {
            Logger::engine_error("Mix_LoadWAV Error: {}", SDL_GetError());
            return false;
        }

        m_Sound = std::unique_ptr<Mix_Chunk, void(*)(Mix_Chunk*)>(chunk, Mix_FreeChunk);
        std::string relativePath = m_WorkingDir + "/" + fileName;

        if (relativePath.find("assets/") == 0) {
            relativePath = relativePath.substr(7);
        }

        ENGINE_LOG_INIT(("Sound: " + relativePath).c_str());
        return true;
    }

    const bool Sound::play(int loops) {
        if (!m_Sound) {
            Logger::engine_error("Sound not loaded!");
            return false;
        }

        if (Mix_PlayChannel(-1, m_Sound.get(), loops) == -1) {
            Logger::engine_error("Mix_PlayChannel Error: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    Mix_Chunk* Sound::getSound() const {
        return m_Sound.get();
    }
}
