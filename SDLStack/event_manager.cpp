#include "include/event_manager.hpp"
#include <iostream>
void event_manager::update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        auto it = listeners.find(event.type);
        if (it != listeners.end())
            for (const auto& listener : it->second) listener(event);
    }
}

void event_manager::add_event_listener(Uint32 type, event_callback callback) {
    listeners[type].push_back(callback);
}