#include "ui.hpp"
#include <iostream>

Button::Button(int x, int y, int w, int h, std::string buttonText) {
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    text = buttonText;
}

void Button::draw(SDL_Renderer* renderer, TTF_Font* font) {
    // Fill background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
    SDL_RenderFillRect(renderer, &rect);

    // Draw the border
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderDrawRect(renderer, &rect);

    // Render text
    if (font == nullptr) {
        // Fallback if font is not available: draw a simple placeholder in the button
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
        SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
        SDL_RenderDrawLine(renderer, rect.x + rect.w, rect.y, rect.x, rect.y + rect.h);
        return;
    }

    SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text.c_str(), textColor);
    if (surfaceMessage == nullptr) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    if (messageTexture == nullptr) {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surfaceMessage);
        return;
    }

    // Center the text inside the button
    SDL_Rect textRect;
    textRect.w = surfaceMessage->w;
    textRect.h = surfaceMessage->h;
    textRect.x = rect.x + (rect.w - textRect.w) / 2;
    textRect.y = rect.y + (rect.h - textRect.h) / 2;

    SDL_RenderCopy(renderer, messageTexture, NULL, &textRect);

    // Clean up
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(messageTexture);
}

bool Button::isClicked(int mouseX, int mouseY) const {
    SDL_Point point = {mouseX, mouseY};
    return SDL_PointInRect(&point, &rect);
}
