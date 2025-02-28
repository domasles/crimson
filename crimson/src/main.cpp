#include <Game.h>

using namespace crimson;

int main() {
    Game game;

    if (!game.init("Crimson", 800, 600)) {
        return -1;
    }

    game.run();
}
