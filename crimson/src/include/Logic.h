#pragma once

#include <SDL3/SDL.h>
#include <Input.h>

namespace crimson {
    class Logic {
        public:
            void init();
            void handleInput();

        private:
            InputSystem m_InputSystem;
    };
}
