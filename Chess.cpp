#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <SDL.h>

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    };
}
