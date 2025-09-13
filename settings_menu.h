#pragma once
#include <SDL2/SDL.h>
#include "ui.hpp"
#include "file_io.h"

// Renders the settings menu
inline void renderSettings(SDL_Renderer* renderer, TTF_Font* font, 
                         Button& invertScrollCheckbox, Button& showCenterCheckbox,
                         Button& importButton, Button& exportButton)
{
    // Clear screen to a dark blue
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);

    // Draw the controls
    invertScrollCheckbox.draw(renderer, font);
    showCenterCheckbox.draw(renderer, font);
    importButton.draw(renderer, font);
    exportButton.draw(renderer, font);
}

// Handles events specifically for the settings menu
inline void handleSettingsEvent(SDL_Event& event, Grid& grid,
                              bool& invertMouseScrolling, Button& invertScrollCheckbox, 
                              bool& showCenterMarker, Button& showCenterCheckbox,
                              Button& importButton, Button& exportButton,
                              std::string& statusMessage, Uint32& statusMessageTimeout)
{
    if (event.type != SDL_MOUSEBUTTONDOWN && event.type != SDL_FINGERUP) return;
    
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    if (invertScrollCheckbox.isClicked(mouseX, mouseY)) {
        invertMouseScrolling = !invertMouseScrolling;
        invertScrollCheckbox.text = invertMouseScrolling ? "[X] Invert Mouse Scrolling" : "[ ] Invert Mouse Scrolling";
    }

    if (showCenterCheckbox.isClicked(mouseX, mouseY)) {
        showCenterMarker = !showCenterMarker;
        showCenterCheckbox.text = showCenterMarker ? "[X] Show Grid Center" : "[ ] Show Grid Center";
    }

    if (importButton.isClicked(mouseX, mouseY)) {
        if (importGrid(grid, "grid_save.txt")) {
            statusMessage = "Grid imported successfully!";
        } else {
            statusMessage = "Error: Failed to import grid.";
        }
        statusMessageTimeout = SDL_GetTicks() + 4000; // Show for 4 seconds
    }

    if (exportButton.isClicked(mouseX, mouseY)) {
        if (exportGrid(grid, "grid_save.txt")) {
            statusMessage = "Grid exported successfully!";
        } else {
            statusMessage = "Error: Failed to export grid.";
        }
        statusMessageTimeout = SDL_GetTicks() + 4000; // Show for 4 seconds
    }
}
