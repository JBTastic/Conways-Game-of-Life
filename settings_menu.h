#pragma once
#include <SDL2/SDL.h>
#include "ui.hpp"

// Renders the settings menu
inline void renderSettings(SDL_Renderer* renderer, TTF_Font* font, Button& invertScrollCheckbox, Button& showCenterCheckbox)
{
    // Clear screen to a dark blue
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);

    // Draw the checkboxes
    invertScrollCheckbox.draw(renderer, font);
    showCenterCheckbox.draw(renderer, font);
}

// Handles events specifically for the settings menu
inline void handleSettingsEvent(SDL_Event& event, 
                              bool& invertMouseScrolling, Button& invertScrollCheckbox, 
                              bool& showCenterMarker, Button& showCenterCheckbox) 
{
    if (event.type != SDL_MOUSEBUTTONDOWN && event.type != SDL_FINGERUP) return;
    
    int mouseX, mouseY;
    // The main loop is responsible for converting touch coordinates to mouse coordinates
    SDL_GetMouseState(&mouseX, &mouseY);

    if (invertScrollCheckbox.isClicked(mouseX, mouseY)) {
        invertMouseScrolling = !invertMouseScrolling;
        invertScrollCheckbox.text = invertMouseScrolling ? "[X] Invert Mouse Scrolling" : "[ ] Invert Mouse Scrolling";
    }

    if (showCenterCheckbox.isClicked(mouseX, mouseY)) {
        showCenterMarker = !showCenterMarker;
        showCenterCheckbox.text = showCenterMarker ? "[X] Show Grid Center" : "[ ] Show Grid Center";
    }
}
