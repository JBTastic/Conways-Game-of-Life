#ifndef UI_HPP
#define UI_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

// Game states
enum class GameState {
    PRE_GAME,
    RUNNING,
    SETTINGS
};

// Represents a clickable UI button
struct Button {
    SDL_Rect rect;
    std::string text;
    SDL_Color textColor = {255, 255, 255, 255}; // Default white
    SDL_Color borderColor = {255, 255, 255, 255}; // Default white

    // Constructor
    Button(int x, int y, int w, int h, std::string text);

    // Draw the button
    void draw(SDL_Renderer* renderer, TTF_Font* font);

    // Check if the button is clicked
    bool isClicked(int mouseX, int mouseY) const;
};

#endif // UI_HPP
