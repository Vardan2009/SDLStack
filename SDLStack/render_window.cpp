#include "include/render_window.hpp"

static bool preload_checks() {
	if (SDL_Init(SDL_INIT_VIDEO) > 0) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Celerity: Failed to initialize SDL Video", SDL_GetError(), NULL);
		exit(1);
		return false;
	}

	return true;
}

render_window::render_window(
	const char* p_title,
	int p_w,
	int p_h
	) : window(NULL), renderer(NULL), screen_w(p_w), screen_h(p_h) {
		window = SDL_CreateWindow(
			p_title,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_UNDEFINED,
			p_w,
			p_h,
			SDL_WINDOW_SHOWN
		);

		if (!window) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Celerity: Failed to initialize SDL window", SDL_GetError(), NULL);
			exit(1);
		}

		if (!preload_checks()) exit(1);

		renderer = SDL_CreateRenderer(
			window,
			-1,
			SDL_RENDERER_ACCELERATED
		);
}

void render_window::cleanup() const {
	SDL_DestroyWindow(window);
}

void render_window::clear() const {
	SDL_RenderClear(renderer);
}

void render_window::render(SDL_Rect p_rect, int r, int g, int b, int a) const {
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderFillRect(renderer, &p_rect);
}

void render_window::display() const {
	SDL_RenderPresent(renderer);
}