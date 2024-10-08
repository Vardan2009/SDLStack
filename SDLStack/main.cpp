#include <SDL.h>
#include <iostream>
#include <deque>
#include <cmath>
#include "include/render_window.hpp"
#include "include/event_manager.hpp"
#include "include/keyboard_manager.hpp"

// Constants
#define WW 1280
#define WH 780
const int height = 25;
double speed = 200;

// Linear interpolation function
double lerp(double start, double end, float t) {
    return start + t * (end - start);
}

// Structs
struct Color {
    int r, g, b;
};

struct Layer {
    double width, posx;
};

// Global Variables
render_window window("SDL Stack", WW, WH);
event_manager event_mgr;
bool game_running = true;
double initial_width = 500; // Store initial width for resetting
double width, posx;
std::deque<Layer> layers;
bool isright = true;
bool last_clicked = false;
Uint32 t_now, t_old;
double camera_offset = 0;
double flashing_timer_initial = 5;
double flashing_timer = 0;

// Function Declarations
Color hsv_to_rgb(float h, float s, float v);
Color gen_color(int iteration);
void on_quit(SDL_Event e);
void reset_game();
void update_game();

// Main Function
int main(int argc, char* argv[]) {
    event_mgr.add_event_listener(SDL_QUIT, on_quit);
    reset_game(); // Initialize the game
    while (game_running) update_game();
    return 0;
}

// Function Definitions
Color hsv_to_rgb(float h, float s, float v) {
    Color color;
    float c = v * s;
    float x = c * (1 - std::fabs(fmod(h / 60.0, 2) - 1));
    float m = v - c;

    if (h < 60) {
        color = { static_cast<int>((c + m) * 255), static_cast<int>((x + m) * 255), static_cast<int>(m * 255) };
    }
    else if (h < 120) {
        color = { static_cast<int>((x + m) * 255), static_cast<int>((c + m) * 255), static_cast<int>(m * 255) };
    }
    else if (h < 180) {
        color = { static_cast<int>(m * 255), static_cast<int>((c + m) * 255), static_cast<int>((x + m) * 255) };
    }
    else if (h < 240) {
        color = { static_cast<int>(m * 255), static_cast<int>((x + m) * 255), static_cast<int>((c + m) * 255) };
    }
    else if (h < 300) {
        color = { static_cast<int>((x + m) * 255), static_cast<int>(m * 255), static_cast<int>((c + m) * 255) };
    }
    else {
        color = { static_cast<int>((c + m) * 255), static_cast<int>(m * 255), static_cast<int>((x + m) * 255) };
    }

    return color;
}

Color gen_color(int iteration) {
    return hsv_to_rgb(static_cast<float>(iteration % 360), 1.0f, 1.0f);
}

void on_quit(SDL_Event e) {
    window.cleanup();
    SDL_Quit();
    exit(0);
}

void reset_game() {
    width = initial_width;
    speed = 200;
    camera_offset = -(int)(layers.size() * height);
    posx = WW / 2;
    layers.clear();
    layers.push_front(Layer{ width, posx });
    isright = true;
    last_clicked = false;
    t_now = t_old = SDL_GetTicks();
}

void update_game() {
    t_now = SDL_GetTicks();
    double delta_time = (t_now - t_old) / 1000.0;
    // delta_time = delta_time < 0.1 ? 0.1 : delta_time; // Ensure a minimum delta time

    keyboard_manager::update();
    Color c = gen_color(2 * layers.size());
    SDL_SetRenderDrawColor(window.renderer, c.r / 1.5, c.g / 1.5, c.b / 1.5, 255);
    window.clear();
    event_mgr.update();

    // Render the main stack piece
    window.render(SDL_Rect{
        static_cast<int>(posx - width / 2),
        WH / 2 - height / 2 - static_cast<int>(camera_offset), // Adjust for camera offset
        static_cast<int>(width), height
        }, c.r, c.g, c.b, 255);

    // Update position based on direction
    posx += (isright ? speed : -speed) * delta_time;

    // Change direction if out of bounds
    const int minx = WW / 2 - initial_width / 2;
    const int maxx = initial_width / 2 + WW / 2;
    isright = (posx < minx) ? true : (posx > maxx) ? false : isright;

    // Handle spacebar click
    if (keyboard_manager::get_key(SDL_SCANCODE_SPACE) && !last_clicked) {
        if (!layers.empty() && std::abs(posx - layers.front().posx) > width) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "SDL Stack", "Game Over! `OK` to Restart", window.window);
            reset_game(); // Restart the game
        }
        else {
            if (!layers.empty()) {
                width -= std::abs(posx - layers.front().posx);
                posx -= (posx - layers.front().posx) / 2;

                flashing_timer = flashing_timer_initial;

                // Move the camera up when placing a layer
                camera_offset += height; // Increment camera offset
            }
            layers.push_front(Layer{ width, posx });
            posx = minx;
            speed++;
        }
    }

    for (int i = 0; i < layers.size() + 5; i += 1) {
        int ly = WH / 2 + height + layers.size() * height - camera_offset;
        ly -= i * height * 10;
        SDL_RenderDrawLine(window.renderer, 0, ly, WW, ly);
    }

    // Render stack layers
    int y = WH / 2 + height * 2 - static_cast<int>(camera_offset); // Adjust for camera offset
    for (size_t i = 0; i < layers.size(); ++i) {
        Color layer_color = gen_color((layers.size() - i) * 2);
        Layer& l = layers[i];
        if (i == 0 && flashing_timer > 0 && (int)flashing_timer % 2 == 0) layer_color = { 255, 255, 255 };
        window.render(SDL_Rect{
            static_cast<int>(l.posx - l.width / 2),
            y,
            static_cast<int>(l.width), height
            }, layer_color.r, layer_color.g, layer_color.b, 255);
        y += height;
    }

    std::cout << flashing_timer << std::endl;
    if (flashing_timer > 0)
        flashing_timer-=delta_time * 20;

    // Render the bottom layer
    Color ground_color = gen_color(0);
    window.render(SDL_Rect{ 0, y, WW, WH }, ground_color.r, ground_color.g, ground_color.b, 255);
    window.display();

    camera_offset = lerp(camera_offset, 0, delta_time * 5);

    // Update for next iteration
    t_old = t_now;
    last_clicked = keyboard_manager::get_key(SDL_SCANCODE_SPACE);
}
