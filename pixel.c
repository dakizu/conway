#include <SDL2/SDL.h>
#include <stdio.h>

#include "pixel.h"

const PixelColor PIXEL_COLOR_RED = {255, 0, 0};
const PixelColor PIXEL_COLOR_BLACK = {0, 0, 0};

SDL_Window *pixel_window;
SDL_Surface *pixel_surface;
int pixel_surface_lock_active = 0;
int pixel_is_initial_frame_v = 1;
PixelInit pixel_init_data;
PixelLoop loop_flow;

void pixel_set_pixel_data(int x, int y, unsigned int data) {
    unsigned int *target_pixel =
        (unsigned int *)((unsigned char *)pixel_surface->pixels +
                         y * pixel_surface->pitch +
                         x * pixel_surface->format->BytesPerPixel);
    *target_pixel = data;
}

void pixel_set_color(int x, int y, PixelColor color) {
    unsigned int real_color;
    int scaled_x, scaled_y, x_i, y_i;
    if (!pixel_surface_lock_active) {
        SDL_LockSurface(pixel_surface);
    }
    real_color = SDL_MapRGB(pixel_surface->format, color.r, color.g, color.b);

    scaled_x = x * pixel_init_data.pixel_scale;
    scaled_y = y * pixel_init_data.pixel_scale;

    for (x_i = 0; x_i < pixel_init_data.pixel_scale; x_i++) {
        for (y_i = 0; y_i < pixel_init_data.pixel_scale; y_i++) {
            pixel_set_pixel_data(scaled_x + x_i, scaled_y + y_i, real_color);
        }
    }
    if (!pixel_surface_lock_active)
        SDL_UnlockSurface(pixel_surface);
}

int pixel_is_initial_frame() { return pixel_is_initial_frame_v; }
void pixel_refresh_screen() {
    if (pixel_surface_lock_active) {
        printf("Attempted to refresh screen with an active suface lock! "
               "Ignoring and continuing...\n");
        return;
    }
    SDL_UpdateWindowSurface(pixel_window);
}

int SDL_main(int argc, char **argv) {
    int input_received_this_frame;
    SDL_Event event;
    pixel_init_data = pixel_init();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == -1) {
        printf("Couldn't initialize SDL2: %s.\n", SDL_GetError());
        goto cleanup_no_sdl;
    }

    pixel_window =
        SDL_CreateWindow(pixel_init_data.window_title, SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, pixel_init_data.window_width,
                         pixel_init_data.window_height, SDL_WINDOW_SHOWN);
    if (!pixel_window) {
        printf("Couldn't create a window: %s.\n", SDL_GetError());
        goto cleanup_sdl;
    }
    pixel_surface = SDL_GetWindowSurface(pixel_window);
    if (!pixel_surface) {
        printf("Couldn't get the window's surface: %s.\n", SDL_GetError());
        goto cleanup_sdl;
    }

    while (1) {
        input_received_this_frame = 0;
        while (1) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT)
                    goto cleanup_sdl;
                if (event.type == SDL_KEYDOWN) {
                    input_received_this_frame = 1;
                }
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    if (pixel_init_data.mouse_cb) {
                        PixelMouseButton button;
                        int x, y;
                        SDL_GetMouseState(&x, &y);
                        if (event.button.button == SDL_BUTTON_LEFT) button = PIXEL_MOUSE_BUTTON_LEFT;
                        else if (event.button.button == SDL_BUTTON_RIGHT) button = PIXEL_MOUSE_BUTTON_RIGHT;
                        else if (event.button.button == SDL_BUTTON_MIDDLE) button = PIXEL_MOUSE_BUTTON_MIDDLE;
                        else button = PIXEL_MOUSE_BUTTON_UNKNOWN;
                        pixel_init_data.mouse_cb(
                            x / pixel_init_data.pixel_scale,
                            y / pixel_init_data.pixel_scale,
                            button);
                    }
                }
            }
            switch (loop_flow) {
            case PIXEL_LOOP_CONTINUE:
                goto start_frame;
                break;
            case PIXEL_LOOP_WAIT_FOR_INPUT:
                if (input_received_this_frame)
                    goto start_frame;
                break;
            }
        }
    start_frame:

        if (SDL_LockSurface(pixel_surface) == -1) {
            printf("Couldn't lock surface for CPU access: %s.\n",
                   SDL_GetError());
            goto cleanup_sdl;
        }
        pixel_surface_lock_active = 1;
        loop_flow = pixel_loop();
        pixel_surface_lock_active = 0;
        SDL_UnlockSurface(pixel_surface);
        pixel_is_initial_frame_v = 0;

        pixel_refresh_screen();
    }

cleanup_sdl:
    SDL_Quit();
cleanup_no_sdl:
    pixel_term();
    exit(0);
}
