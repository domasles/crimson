#include <pch.h>

#include <Game.h>

using namespace crimson;

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

constexpr int FPS = 144;
constexpr bool VSYNC = true;

int main() {
    if (!Game::getInstance().init("Crimson", WINDOW_WIDTH, WINDOW_HEIGHT, VSYNC)) {
        return -1;
    }

    Game::getInstance().run();
}
