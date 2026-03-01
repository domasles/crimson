#pragma once

#include <utils/filesystem.h>
#include <ui/UIManager.h>

namespace engine {
    inline void setFont(const std::string& path, bool fallback = false) {
        ui::UIManager::getInstance().loadFont(utils::filesystem::getGamePath() + "/" + path, fallback);
    }
}
