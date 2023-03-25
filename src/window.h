#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>

class window {
public:
    int width;
    int height;
    SDL_Window* win = NULL;
    SDL_Renderer* ren = NULL;
    SDL_Texture *texture = NULL;

    window(int w, int h) : width(w), height(h) {
        SDL_Log("Starting Window");
        SDL_Init(SDL_INIT_EVERYTHING);
        if (SDL_CreateWindowAndRenderer(width, height, 0, &win, &ren) == -1) {
            SDL_Log("Unable to create window and renderer: %s", SDL_GetError());
            exit(1);
        }
        // create texture
        texture = SDL_CreateTexture(
            ren,
            SDL_PIXELFORMAT_RGB24,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height
        );
        if (texture == NULL) {
            SDL_Log("Unable to create texture: %s", SDL_GetError());
            exit(1);
        }
        SDL_RaiseWindow(win);
    }

    void update(uint8_t* pixel_arr) {
        // update texture with new data
        int texture_pitch = 0;
        void* texture_pixels = NULL;
        if (SDL_LockTexture(texture, NULL, &texture_pixels, &texture_pitch) != 0) {
            SDL_Log("Unable to lock texture: %s", SDL_GetError());
        }
        else {
            memcpy(texture_pixels, pixel_arr, texture_pitch * height);
        }
        SDL_UnlockTexture(texture);
        SDL_RenderCopyEx(ren, texture, nullptr, nullptr, 0, nullptr, SDL_FLIP_VERTICAL);
        SDL_RenderPresent(ren);
    }

    void shutdown() {
        SDL_Log("Closing Window");
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_DestroyTexture(texture);
        SDL_Quit();
    }
};


#endif