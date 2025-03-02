#include <Game.h>

using namespace crimson;

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

int main() {
    if (!Game::getInstance().init("Crimson", WINDOW_WIDTH, WINDOW_HEIGHT)) {
        return -1;
    }

    Game::getInstance().run();
}
