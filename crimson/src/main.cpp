#include <Game.h>

using namespace crimson;

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

int main() {
    Game game;

    if (!game.init("Crimson", WINDOW_WIDTH, WINDOW_HEIGHT)) {
        return -1;
    }

    game.run();
}
