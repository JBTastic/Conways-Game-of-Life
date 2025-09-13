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
const float MIN_CELL_SIZE_F = 2.0f;
const float MAX_CELL_SIZE_F = 100.0f;

// HELPER FUNCTION FOR ZOOMING
void zoom(Grid &grid, float &preciseCellSize, float zoomFactor, int mouseX, int mouseY) {
    // Get world coordinates of the point under the mouse
    float worldX = (float)(mouseX - grid.offsetX) / grid.cellSize;
    float worldY = (float)(mouseY - grid.offsetY) / grid.cellSize;

    // Update precise cell size
    preciseCellSize *= zoomFactor;
    if (preciseCellSize < MIN_CELL_SIZE_F) preciseCellSize = MIN_CELL_SIZE_F;
    if (preciseCellSize > MAX_CELL_SIZE_F) preciseCellSize = MAX_CELL_SIZE_F;

    int newCellSize = static_cast<int>(preciseCellSize);

    // If the integer cell size has not changed, no need to update offsets or redraw
    if (newCellSize == grid.cellSize) return;

    grid.cellSize = newCellSize;

    // Adjust offset to keep the world point under the mouse
    grid.offsetX = mouseX - (int)(worldX * grid.cellSize);
    grid.offsetY = mouseY - (int)(worldY * grid.cellSize);
}


int main(int argc, char *argv[]) {
    // INITIALIZATION
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
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);


    // FONT AND UI
    TTF_Font *font = TTF_OpenFont("sans.ttf", 24);

    Button settingsButton(10, 10, 150, 40, "Settings");
    Button startButton(SCREEN_WIDTH - 160, 10, 150, 40, "Start");
    Button clearButton(SCREEN_WIDTH / 2 - 75, 10, 150, 40, "Clear");

    // GAME SETUP
    GameState currentState = GameState::PRE_GAME;
    Grid grid = initGrid(200, 200, 20);
    float preciseCellSize = (float)grid.cellSize;
    InputState input;
    bool running = true;
    Uint32 lastUpdateTime = 0;
    const Uint32 updateInterval = 100;

    // MAIN LOOP
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            bool eventHandled = false;

            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    eventHandled = true;
                    break;

                // TOUCH AND MOUSE CLICK HANDLING
                case SDL_MOUSEBUTTONDOWN:
                case SDL_FINGERUP: {
                    int mouseX, mouseY;
                    if (event.type == SDL_MOUSEBUTTONDOWN) {
                        mouseX = event.button.x;
                        mouseY = event.button.y;
                    } else { // Fingerup
                        int w, h;
                        SDL_GetWindowSize(window, &w, &h);
                        mouseX = event.tfinger.x * w;
                        mouseY = event.tfinger.y * h;
                    }

                    if (settingsButton.isClicked(mouseX, mouseY)) {
                        currentState = (currentState == GameState::SETTINGS) ? GameState::PRE_GAME : GameState::SETTINGS;
                        settingsButton.text = (currentState == GameState::SETTINGS) ? "Back" : "Settings";
                        eventHandled = true;
                    } else if (startButton.isClicked(mouseX, mouseY)) {
                        currentState = (currentState == GameState::RUNNING) ? GameState::PRE_GAME : GameState::RUNNING;
                        startButton.text = (currentState == GameState::RUNNING) ? "Pause" : "Start";
                        eventHandled = true;
                    } else if (clearButton.isClicked(mouseX, mouseY) && currentState == GameState::PRE_GAME) {
                        clearGrid(grid);
                        eventHandled = true;
                    }
                    break;
                }

                // ZOOM AND PAN HANDLING
                case SDL_MOUSEWHEEL: {
                    SDL_Keymod mod = SDL_GetModState();
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    if (mod & KMOD_CTRL) { // Zoom with ctrl+scroll
                        float zoomFactor = (event.wheel.y > 0) ? 1.1f : 0.9f;
                        zoom(grid, preciseCellSize, zoomFactor, mouseX, mouseY);
                    } else { // Pan with scroll
                        grid.offsetX -= event.wheel.x * (grid.cellSize / 2);
                        grid.offsetY += event.wheel.y * (grid.cellSize / 2);
                    }
                    eventHandled = true;
                    break;
                }

                case SDL_KEYDOWN: {
                    SDL_Keymod mod = SDL_GetModState();
                    if (mod & KMOD_CTRL) {
                        if (event.key.keysym.sym == SDLK_EQUALS || event.key.keysym.sym == SDLK_PLUS) {
                            int w, h;
                            SDL_GetWindowSize(window, &w, &h);
                            zoom(grid, preciseCellSize, 1.25f, w / 2, h / 2);
                            eventHandled = true;
                        } else if (event.key.keysym.sym == SDLK_MINUS) {
                            int w, h;
                            SDL_GetWindowSize(window, &w, &h);
                            zoom(grid, preciseCellSize, 0.8f, w / 2, h / 2);
                            eventHandled = true;
                        }
                    }
                    break;
                }

                case SDL_MULTIGESTURE: {
                    input.inMultiGesture = true;
                    if (event.mgesture.dDist > 0.0001f || event.mgesture.dDist < -0.0001f) { // Lowered threshold
                        int w, h;
                        SDL_GetWindowSize(window, &w, &h);
                        float zoomFactor = 1.0f + (event.mgesture.dDist * 2.0f); // Amplified factor
                        zoom(grid, preciseCellSize, zoomFactor, event.mgesture.x * w, event.mgesture.y * h);
                        eventHandled = true;
                    }
                    break;
                }
            }

            // STATE-SPECIFIC EVENT HANDLING
            if (!eventHandled) {
                switch (currentState) {
                    case GameState::PRE_GAME:
                        handlePreGameEvent(event, grid, input, window, true);
                        break;
                    case GameState::RUNNING:
                        handlePreGameEvent(event, grid, input, window, false); // Allow panning/dragging but not toggling
                        break;
                    case GameState::SETTINGS:
                        handleSettingsEvent(event);
                        break;
                    default: break;
                }
            }
        }

        // LOGIC AND UPDATES
        if (currentState == GameState::RUNNING) {
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - lastUpdateTime > updateInterval) {
                updateGrid(grid);
                lastUpdateTime = currentTime;
            }
        }

        // RENDERING
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        startButton.rect.x = w - 160;
        clearButton.rect.x = w / 2 - 75;

        SDL_SetRenderDrawColor(renderer, 10, 10, 20, 255);
        SDL_RenderClear(renderer);

        switch (currentState) {
            case GameState::PRE_GAME:
            case GameState::RUNNING:
                renderGrid(renderer, grid);
                break;
            case GameState::SETTINGS:
                renderSettings(renderer, font);
                break;
        }
        
        // Set clear button color based on state
        bool isClearEnabled = (currentState == GameState::PRE_GAME);
        SDL_Color disabledColor = {100, 100, 100, 255};
        SDL_Color enabledColor = {255, 255, 255, 255};
        clearButton.borderColor = isClearEnabled ? enabledColor : disabledColor;
        clearButton.textColor = isClearEnabled ? enabledColor : disabledColor;

        settingsButton.draw(renderer, font);
        startButton.draw(renderer, font);
        clearButton.draw(renderer, font);

        SDL_RenderPresent(renderer);
    }

    // CLEANUP
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}