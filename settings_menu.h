#pragma once
#include <SDL2/SDL.h>
#include "ui.hpp"

inline void renderSettings(SDL_Renderer* renderer, TTF_Font* font)
{
    // Clear screen to a dark blue
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);

    // TODO: Add settings options here
}

inline void handleSettingsEvent(SDL_Event& event) {
    // TODO: Handle events for the settings menu
}
