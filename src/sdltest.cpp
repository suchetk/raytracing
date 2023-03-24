#include <stdlib.h>
#include <iostream>
#include <SDL2/SDL.h>

void update(SDL_Renderer* ren, SDL_Texture* texture, uint8_t pixel_arr[], int height) {
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
    SDL_RenderCopy(ren, texture, nullptr, nullptr);
    SDL_RenderPresent(ren);
}

void set_pixels(uint8_t* pixels, int x, int width, int height) {
    for (int i = 0; i<width*height; i++) {
        pixels[4*i] = (250-x) % 256;
        pixels[4*i+1] = (250+x) % 256;
        pixels[4*i+2] = (100+x) % 256;
    }
}

int main(void) {
    SDL_Window* win = NULL;
    SDL_Renderer* ren = NULL;
    SDL_Init(SDL_INIT_EVERYTHING);
    int width = 1080;
    int height = 607;
    if (SDL_CreateWindowAndRenderer(width, height, 0, &win, &ren) == -1) {
        SDL_Log("Unable to create window and renderer: %s", SDL_GetError());
        return 1;
    }

    // create texture
    SDL_Texture *texture = SDL_CreateTexture(
        ren,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height);

    if (texture == NULL) {
        SDL_Log("Unable to create texture: %s", SDL_GetError());
        return 1;
    }

    // array of pixels
    uint8_t pixels[width * height * 4];
    memset(pixels, 255, width*height*4);

    int x = 0;
    set_pixels(pixels, x, width, height);
    update(ren, texture, pixels, height);

    //Event handler
    SDL_Event e;

    //While application is running
    bool quit = false;
    while( !quit )
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 ) // poll for event
        {
            //User requests quit
            if( e.type == SDL_QUIT ) // unless player manually quits
            {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                std::cout << e.key.keysym.sym << std::endl;
            }
        }
        x += 1;
        set_pixels(pixels, x, width, height);
        update(ren, texture, pixels, height);
    }



    return (0);
}