#include <iostream>
#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h> // For loading PNGs for the icon

#include "ui.hpp"
#include "pre_game.h"
#include "game_logic.h"
#include "settings_menu.h"
#include "file_io.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const float MIN_CELL_SIZE_F = 4.0f;
const float MAX_CELL_SIZE_F = 100.0f;

// HELPER FUNCTION FOR ZOOMING
void zoom(Grid &grid, float &preciseCellSize, float zoomFactor, int mouseX, int mouseY)
{
    // Get world coordinates of the point under the mouse
    float worldX = (float)(mouseX - grid.offsetX) / grid.cellSize;
    float worldY = (float)(mouseY - grid.offsetY) / grid.cellSize;

    // Update precise cell size
    preciseCellSize *= zoomFactor;
    if (preciseCellSize < MIN_CELL_SIZE_F)
        preciseCellSize = MIN_CELL_SIZE_F;
    if (preciseCellSize > MAX_CELL_SIZE_F)
        preciseCellSize = MAX_CELL_SIZE_F;

    int newCellSize = static_cast<int>(preciseCellSize);

    // If the integer cell size has not changed, no need to update offsets or redraw
    if (newCellSize == grid.cellSize)
        return;

    grid.cellSize = newCellSize;

    // Adjust offset to keep the world point under the mouse
    grid.offsetX = mouseX - (int)(worldX * grid.cellSize);
    grid.offsetY = mouseY - (int)(worldY * grid.cellSize);
}

// Helper to render text
void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color) {
    if (!font || text.empty()) return;
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }
    SDL_Rect destRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

int main(int argc, char *argv[])
{
    // INITIALIZATION
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    if (TTF_Init() == -1) return 1;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) { /* Log error but continue */ }

    SDL_Window *window = SDL_CreateWindow("Conway's Game of Life",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Set App Icon
    SDL_Surface* iconSurface = IMG_Load("icon.png");
    if (iconSurface) {
        SDL_SetWindowIcon(window, iconSurface);
        SDL_FreeSurface(iconSurface);
    }

    // FONT AND UI
    TTF_Font *font = TTF_OpenFont("sans.ttf", 24);

    // UI Elements
    Button settingsButton(10, 10, 150, 40, "Settings");
    Button startButton(SCREEN_WIDTH - 160, 10, 150, 40, "Start");
    Button clearButton(SCREEN_WIDTH / 2 - 165, 10, 150, 40, "Clear");
    Button centerButton(SCREEN_WIDTH / 2 + 15, 10, 150, 40, "Center");
    // Settings UI Elements
    Button invertScrollCheckbox(100, 100, 450, 40, "[ ] Invert Mouse Scrolling");
    Button showCenterCheckbox(100, 150, 450, 40, "[X] Show Grid Center");
    Button importButton(100, 250, 200, 40, "Import Grid");
    Button exportButton(310, 250, 200, 40, "Export Grid");

    // GAME AND APP STATE
    GameState currentState = GameState::PRE_GAME;
    Grid grid = initGrid(200, 200, 20);
    float preciseCellSize = (float)grid.cellSize;
    InputState input;
    bool running = true;
    Uint32 lastUpdateTime = 0;
    const Uint32 updateInterval = 100;
    bool invertMouseScrolling = false;
    bool showCenterMarker = true;
    std::string statusMessage = "";
    Uint32 statusMessageTimeout = 0;

    // MAIN LOOP
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            bool eventHandled = false;

            // Global non-state-specific events
            switch (event.type)
            {
                case SDL_QUIT: running = false; eventHandled = true; break;
                case SDL_KEYDOWN: {
                    SDL_Keymod mod = SDL_GetModState();
                    if (mod & KMOD_CTRL) {
                        if (event.key.keysym.sym == SDLK_EQUALS || event.key.keysym.sym == SDLK_PLUS) {
                            int w, h; SDL_GetWindowSize(window, &w, &h);
                            zoom(grid, preciseCellSize, 1.25f, w / 2, h / 2);
                            eventHandled = true;
                        } else if (event.key.keysym.sym == SDLK_MINUS) {
                            int w, h; SDL_GetWindowSize(window, &w, &h);
                            zoom(grid, preciseCellSize, 0.8f, w / 2, h / 2);
                            eventHandled = true;
                        }
                    }
                    break;
                }
                case SDL_MULTIGESTURE: {
                    input.inMultiGesture = true;
                    if (event.mgesture.dDist > 0.0001f || event.mgesture.dDist < -0.0001f) { 
                        int w, h; SDL_GetWindowSize(window, &w, &h);
                        float zoomFactor = 1.0f + (event.mgesture.dDist * 8.0f);
                        zoom(grid, preciseCellSize, zoomFactor, event.mgesture.x * w, event.mgesture.y * h);
                        eventHandled = true;
                    }
                    break;
                }
                case SDL_MOUSEWHEEL: {
                    SDL_Keymod mod = SDL_GetModState();
                    int mouseX, mouseY; SDL_GetMouseState(&mouseX, &mouseY);
                    int scroll_direction = invertMouseScrolling ? -1 : 1;
                    int wheel_y = event.wheel.y * scroll_direction;
                    int wheel_x = event.wheel.x * scroll_direction;

                    if (mod & KMOD_CTRL) { // Zoom
                        float zoomFactor = (wheel_y > 0) ? 1.1f : 0.9f;
                        zoom(grid, preciseCellSize, zoomFactor, mouseX, mouseY);
                    } else { // Pan
                        const int PAN_SPEED = 40;
                        int dx = 0, dy = 0;
                        if (mod & KMOD_SHIFT) { dx = -wheel_y * PAN_SPEED; } 
                        else { dx = -wheel_x * PAN_SPEED; dy = wheel_y * PAN_SPEED; }
                        int w, h; SDL_GetWindowSize(window, &w, &h);
                        panGrid(grid, dx, dy, w, h);
                    }
                    eventHandled = true;
                    break;
                }
            }

            if (eventHandled) continue;

            // Handle clicks, which are state-dependent
            if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_FINGERUP) {
                int mouseX, mouseY;
                if (event.type == SDL_MOUSEBUTTONDOWN) { mouseX = event.button.x; mouseY = event.button.y; }
                else { int w, h; SDL_GetWindowSize(window, &w, &h); mouseX = event.tfinger.x * w; mouseY = event.tfinger.y * h; }
                
                if (settingsButton.isClicked(mouseX, mouseY)) {
                    currentState = (currentState == GameState::SETTINGS) ? GameState::PRE_GAME : GameState::SETTINGS;
                    settingsButton.text = (currentState == GameState::SETTINGS) ? "Back" : "Settings";
                    eventHandled = true;
                }

                if (currentState == GameState::SETTINGS) {
                    handleSettingsEvent(event, grid, invertMouseScrolling, invertScrollCheckbox, showCenterMarker, showCenterCheckbox, importButton, exportButton, statusMessage, statusMessageTimeout);
                } else {
                    if (startButton.isClicked(mouseX, mouseY)) {
                        currentState = (currentState == GameState::RUNNING) ? GameState::PRE_GAME : GameState::RUNNING;
                        startButton.text = (currentState == GameState::RUNNING) ? "Pause" : "Start";
                        eventHandled = true;
                    } else if (clearButton.isClicked(mouseX, mouseY) && currentState == GameState::PRE_GAME) {
                        clearGrid(grid);
                        eventHandled = true;
                    } else if (centerButton.isClicked(mouseX, mouseY)) {
                        int w, h; SDL_GetWindowSize(window, &w, &h);
                        jumpToCenter(grid, w, h);
                        eventHandled = true;
                    }
                }
            }

            if (eventHandled) continue;

            // Pass all other unhandled events to state-specific handlers
            switch (currentState) {
                case GameState::PRE_GAME: handlePreGameEvent(event, grid, input, window, true); break;
                case GameState::RUNNING: handlePreGameEvent(event, grid, input, window, false); break;
                case GameState::SETTINGS: break;
            }
        }

        // LOGIC AND UPDATES
        if (currentState == GameState::RUNNING) {
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - lastUpdateTime > updateInterval) { updateGrid(grid); lastUpdateTime = currentTime; }
        }
        // Status message timeout
        if (SDL_GetTicks() > statusMessageTimeout) {
            statusMessage = "";
        }

        // RENDERING
        int w, h; SDL_GetWindowSize(window, &w, &h);
        SDL_SetRenderDrawColor(renderer, 10, 10, 20, 255); SDL_RenderClear(renderer);

        if (currentState == GameState::PRE_GAME || currentState == GameState::RUNNING) {
            renderGrid(renderer, grid, showCenterMarker);
        } else { // SETTINGS
            renderSettings(renderer, font, invertScrollCheckbox, showCenterCheckbox, importButton, exportButton);
        }

        // Draw UI on top
        startButton.rect.x = w - 160;
        clearButton.rect.x = w / 2 - 165;
        centerButton.rect.x = w / 2 + 15;
        settingsButton.draw(renderer, font);
        
        if (currentState != GameState::SETTINGS) {
            bool isClearEnabled = (currentState == GameState::PRE_GAME);
            SDL_Color disabledColor = {100, 100, 100, 255}; SDL_Color enabledColor = {255, 255, 255, 255};
            clearButton.borderColor = isClearEnabled ? enabledColor : disabledColor;
            clearButton.textColor = isClearEnabled ? enabledColor : disabledColor;
            
            startButton.draw(renderer, font);
            clearButton.draw(renderer, font);
            centerButton.draw(renderer, font);
        } else {
            // Render status message in settings
            renderText(renderer, font, statusMessage, 100, h - 50, {255, 255, 0, 255});
        }

        SDL_RenderPresent(renderer);
    }

    // CLEANUP
    TTF_CloseFont(font);
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
