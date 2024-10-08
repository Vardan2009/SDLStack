#include "include/keyboard_manager.hpp"

const Uint8 *keyboard_manager::kb_state = nullptr;

void keyboard_manager::update() {
	kb_state = SDL_GetKeyboardState(NULL);
}

bool keyboard_manager::get_key(SDL_Scancode p_scancode) {
	return kb_state[p_scancode];
}