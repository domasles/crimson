#include "include/Game.h"

int main() {
    crimson::Game game;

    if (!game.init("Crimson", 800, 600)) {
        return -1;
    }

    game.run();
}
