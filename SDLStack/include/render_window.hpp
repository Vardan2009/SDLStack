#pragma once
#include <SDL.h>
#include <iostream>
#include "common.hpp"

class entity;

class render_window {
public:
	render_window(
		const char* p_title,
		int p_w,
		int p_h
	);
	int screen_w, screen_h;
	void cleanup() const;
	void render(SDL_Rect p_rect, int r, int g, int b, int a) const;
	void clear() const;
	void display() const;

	point camera_pos{ 0, 0 };

	SDL_Window* window;
	SDL_Renderer* renderer;
};