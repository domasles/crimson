#pragma once

#include <utils/filesystem.h>
#include <ui/UIContext.h>

namespace engine {
    inline ui::UIDocument loadDocument(const std::string& path) {
        return ui::getCurrentContext().loadDocument(utils::filesystem::getGamePath() + "/" + path);
    }
}
