#pragma once
#include <set>
#include "event_manager.hpp"

class keyboard_manager {
public:
	static void update();
	static bool get_key(SDL_Scancode);
private:
	static const Uint8 *kb_state;
};
