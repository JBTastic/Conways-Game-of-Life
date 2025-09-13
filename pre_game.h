#pragma once
#include <SDL2/SDL.h>
#include <vector>

struct Grid
{
    std::vector<std::vector<bool>> cells;
    int rows;
    int cols;
    int cellSize;
    int offsetX;
    int offsetY;
};

struct InputState
{
    bool rightMouseDown = false;
    int lastMouseX = 0;
    int lastMouseY = 0;

    bool fingerDown = false;
    bool isPanning = false;
    bool inMultiGesture = false; // To prevent conflicts between gesture and motion
    float startFingerX = 0.0f;
    float startFingerY = 0.0f;
    float lastFingerX = 0.0f;
    float lastFingerY = 0.0f;
};

// Initialize grid
inline Grid initGrid(int rows, int cols, int cellSize)
{
    return Grid{std::vector<std::vector<bool>>(rows, std::vector<bool>(cols, false)),
                rows, cols, cellSize, 0, 0};
}

// Clear all cells in the grid
inline void clearGrid(Grid &grid) {
    for (int row = 0; row < grid.rows; ++row) {
        for (int col = 0; col < grid.cols; ++col) {
            grid.cells[row][col] = false;
        }
    }
}

// Toggle cell at position
inline void toggleCell(Grid &grid, int x, int y)
{
    int col = x / grid.cellSize;
    int row = y / grid.cellSize;
    if (row >= 0 && row < grid.rows && col >= 0 && col < grid.cols)
        grid.cells[row][col] = !grid.cells[row][col];
}

// Render the grid
inline void renderGrid(SDL_Renderer *renderer, const Grid &grid)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int row = 0; row < grid.rows; row++)
    {
        for (int col = 0; col < grid.cols; col++)
        {
            if (grid.cells[row][col])
            {
                SDL_Rect cellRect = {col * grid.cellSize + grid.offsetX,
                                     row * grid.cellSize + grid.offsetY,
                                     grid.cellSize, grid.cellSize};
                SDL_RenderFillRect(renderer, &cellRect);
            }
        }
    }

    // Draw grid lines
    for (int x = 0; x <= grid.cols * grid.cellSize; x += grid.cellSize)
        SDL_RenderDrawLine(renderer, x + grid.offsetX, 0 + grid.offsetY,
                           x + grid.offsetX, grid.rows * grid.cellSize + grid.offsetY);

    for (int y = 0; y <= grid.rows * grid.cellSize; y += grid.cellSize)
        SDL_RenderDrawLine(renderer, 0 + grid.offsetX, y + grid.offsetY,
                           grid.cols * grid.cellSize + grid.offsetX, y + grid.offsetY);
}

// Handle mouse and touch events for pre-game (placing cells + panning)
inline void handlePreGameEvent(SDL_Event &event, Grid &grid, InputState &input, SDL_Window *window, bool allowToggle)
{
    const float PAN_THRESHOLD = 0.005f;

    switch (event.type)
    {
    case SDL_MOUSEBUTTONDOWN:
        if (allowToggle && event.button.button == SDL_BUTTON_LEFT)
        {
            toggleCell(grid, event.button.x - grid.offsetX, event.button.y - grid.offsetY);
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
            input.rightMouseDown = true;
            input.lastMouseX = event.button.x;
            input.lastMouseY = event.button.y;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (event.button.button == SDL_BUTTON_RIGHT)
            input.rightMouseDown = false;
        break;
    case SDL_MOUSEMOTION:
        if (input.rightMouseDown)
        {
            int dx = event.motion.x - input.lastMouseX;
            int dy = event.motion.y - input.lastMouseY;
            grid.offsetX += dx;
            grid.offsetY += dy;
            input.lastMouseX = event.motion.x;
            input.lastMouseY = event.motion.y;
        }
        break;
    case SDL_FINGERDOWN:
        if (SDL_GetNumTouchFingers(event.tfinger.touchId) == 1) {
            input.inMultiGesture = false; // Reset on first finger down
        }
        input.fingerDown = true;
        input.isPanning = false;
        input.startFingerX = input.lastFingerX = event.tfinger.x;
        input.startFingerY = input.lastFingerY = event.tfinger.y;
        break;
    case SDL_FINGERUP:
        if (allowToggle && !input.isPanning && !input.inMultiGesture)
        {
            int w, h;
            SDL_GetWindowSize(window, &w, &h);
            toggleCell(grid, input.startFingerX * w - grid.offsetX, input.startFingerY * h - grid.offsetY);
        }
        input.fingerDown = false;
        input.isPanning = false;
        // Only reset gesture flag when the last finger is lifted
        if (SDL_GetNumTouchFingers(event.tfinger.touchId) == 0) {
            input.inMultiGesture = false;
        }
        break;
    case SDL_FINGERMOTION:
        if (input.fingerDown && !input.inMultiGesture)
        {
            float dxf = event.tfinger.x - input.startFingerX;
            float dyf = event.tfinger.y - input.startFingerY;
            if (!input.isPanning && (std::abs(dxf) > PAN_THRESHOLD || std::abs(dyf) > PAN_THRESHOLD))
                input.isPanning = true;
            if (input.isPanning)
            {
                int w, h;
                SDL_GetWindowSize(window, &w, &h);
                int dx = (event.tfinger.x - input.lastFingerX) * w;
                int dy = (event.tfinger.y - input.lastFingerY) * h;
                grid.offsetX += dx;
                grid.offsetY += dy;
                input.lastFingerX = event.tfinger.x;
                input.lastFingerY = event.tfinger.y;
            }
        }
        break;
    }
}