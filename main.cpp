#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "ui.hpp"
#include "pre_game.h"
#include "game_logic.h"
#include "settings_menu.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

int main(int argc, char *argv[]) {
    // --- INITIALIZATION ---
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not be initialized! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not be initialized! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Conway's Game of Life",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // --- FONT AND UI ---
    // IMPORTANT: Place a font file named 'sans.ttf' in the same directory as the executable
    TTF_Font *font = TTF_OpenFont("sans.ttf", 24);
    if (font == nullptr) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        // We don't exit, the button drawing function has a fallback
    }

    Button settingsButton(10, 10, 150, 40, "Settings");
    Button startButton(SCREEN_WIDTH - 160, 10, 150, 40, "Start");


    // --- GAME SETUP ---
    GameState currentState = GameState::PRE_GAME;
    const int cellSize = 20;
    Grid grid = initGrid(100, 100, cellSize); // A large grid
    InputState input;
    
    bool running = true;
    Uint32 lastUpdateTime = 0;
    const Uint32 updateInterval = 100; // Milliseconds between updates

    // --- MAIN LOOP ---
    while (running) {
        SDL_Event event;
        // --- EVENT HANDLING ---
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            // --- Global Button Click Handling ---
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                if (settingsButton.isClicked(mouseX, mouseY)) {
                    if (currentState == GameState::SETTINGS) {
                        currentState = GameState::PRE_GAME;
                        settingsButton.text = "Settings";
                    } else {
                        currentState = GameState::SETTINGS;
                        settingsButton.text = "Back";
                    }
                } else if (startButton.isClicked(mouseX, mouseY)) {
                    if (currentState == GameState::RUNNING) {
                        currentState = GameState::PRE_GAME;
                        startButton.text = "Start";
                    } else {
                        currentState = GameState::RUNNING;
                        startButton.text = "Pause";
                    }
                } else {
                    // If no button was clicked, pass event to state-specific handler
                    switch (currentState) {
                        case GameState::PRE_GAME:
                            handlePreGameEvent(event, grid, input, window);
                            break;
                        case GameState::SETTINGS:
                            handleSettingsEvent(event);
                            break;
                        case GameState::RUNNING:
                            // No input during simulation for now
                            break;
                    }
                }
            } else {
                 // Pass non-click events to handlers as well
                 if (currentState == GameState::PRE_GAME) {
                    handlePreGameEvent(event, grid, input, window);
                 }
            }
        }

        // --- LOGIC / UPDATES ---
        if (currentState == GameState::RUNNING) {
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - lastUpdateTime > updateInterval) {
                updateGrid(grid);
                lastUpdateTime = currentTime;
            }
        }

        // --- RENDERING ---
        // Get window size for responsive UI
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        startButton.rect.x = w - 160; // Keep start button on the right

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 10, 10, 20, 255);
        SDL_RenderClear(renderer);

        // State-specific rendering
        switch (currentState) {
            case GameState::PRE_GAME:
            case GameState::RUNNING:
                renderGrid(renderer, grid);
                break;
            case GameState::SETTINGS:
                renderSettings(renderer, font);
                break;
        }

        // Draw UI on top of everything
        settingsButton.draw(renderer, font);
        startButton.draw(renderer, font);

        // Present frame
        SDL_RenderPresent(renderer);
    }

    // --- CLEANUP ---
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}