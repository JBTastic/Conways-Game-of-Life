#pragma once
#include <SDL2/SDL.h>
#include "ui.hpp"

// Renders the settings menu
inline void renderSettings(SDL_Renderer* renderer, TTF_Font* font, Button& invertScrollCheckbox)
{
    // Clear screen to a dark blue
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);

    // Draw the checkbox
    invertScrollCheckbox.draw(renderer, font);
}

// Handles events specifically for the settings menu
inline void handleSettingsEvent(SDL_Event& event, bool& invertMouseScrolling, Button& invertScrollCheckbox) {
    // We only care about clicks for the checkbox
    if (event.type != SDL_MOUSEBUTTONDOWN && event.type != SDL_FINGERUP) return;
    
    int mouseX, mouseY;
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        mouseX = event.button.x;
        mouseY = event.button.y;
    } else { // Fingerup, needs coordinate conversion from normalized
        // This part is tricky as we don't have window size here. 
        // The main event loop should handle coordinate conversion.
        // For now, we assume the main loop has prepared the mouse coordinates for us.
        // Let's just get the state. This will be handled in main.cpp
        SDL_GetMouseState(&mouseX, &mouseY);
    }

    if (invertScrollCheckbox.isClicked(mouseX, mouseY)) {
        invertMouseScrolling = !invertMouseScrolling;
        // Update checkbox text to reflect state
        if (invertMouseScrolling) {
            invertScrollCheckbox.text = "[X] Invert Mouse Scrolling";
        } else {
            invertScrollCheckbox.text = "[ ] Invert Mouse Scrolling";
        }
    }
}