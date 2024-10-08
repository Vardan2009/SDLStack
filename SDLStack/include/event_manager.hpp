#pragma once
#include <SDL.h>
#include <unordered_map>
#include <vector>

typedef void (*event_callback)(SDL_Event);

class event_manager {
public:
	void update();
	void add_event_listener(Uint32, event_callback);
private:
	std::unordered_map<Uint32, std::vector<event_callback>> listeners;
};