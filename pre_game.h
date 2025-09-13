#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <algorithm> // For std::min/max

const int MIN_CELL_SIZE_FOR_TOGGLE = 8;

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

// Jumps the camera to the center of the grid
inline void jumpToCenter(Grid &grid, int windowWidth, int windowHeight)
{
    int gridCenterPixelX = (grid.cols / 2) * grid.cellSize;
    int gridCenterPixelY = (grid.rows / 2) * grid.cellSize;

    grid.offsetX = windowWidth / 2 - gridCenterPixelX;
    grid.offsetY = windowHeight / 2 - gridCenterPixelY;
}

// Centralized function to handle panning with constraints
inline void panGrid(Grid &grid, int dx, int dy, int windowWidth, int windowHeight)
{
    int gridPixelWidth = grid.cols * grid.cellSize;
    int gridPixelHeight = grid.rows * grid.cellSize;

    // Define the soft boundaries
    int minX = -gridPixelWidth + grid.cellSize;
    int maxX = windowWidth - grid.cellSize;
    int minY = -gridPixelHeight + grid.cellSize;
    int maxY = windowHeight - grid.cellSize;

    // If the grid is smaller than the window, center it and don't pan
    if (gridPixelWidth < windowWidth)
    {
        grid.offsetX = (windowWidth - gridPixelWidth) / 2;
    }
    else
    {
        grid.offsetX = std::max(minX, std::min(maxX, grid.offsetX + dx));
    }

    if (gridPixelHeight < windowHeight)
    {
        grid.offsetY = (windowHeight - gridPixelHeight) / 2;
    }
    else
    {
        grid.offsetY = std::max(minY, std::min(maxY, grid.offsetY + dy));
    }
}

// Initialize grid
inline Grid initGrid(int rows, int cols, int cellSize)
{
    return Grid{std::vector<std::vector<bool>>(rows, std::vector<bool>(cols, false)),
                rows, cols, cellSize, 0, 0};
}

// Clear all cells in the grid
inline void clearGrid(Grid &grid)
{
    for (int row = 0; row < grid.rows; ++row)
    {
        for (int col = 0; col < grid.cols; ++col)
        {
            grid.cells[row][col] = false;
        }
    }
}

// Toggle cell at position
inline void toggleCell(Grid &grid, int x, int y)
{
    // Do not allow toggling if zoomed out too far
    if (grid.cellSize < MIN_CELL_SIZE_FOR_TOGGLE)
        return;

    int col = x / grid.cellSize;
    int row = y / grid.cellSize;
    if (row >= 0 && row < grid.rows && col >= 0 && col < grid.cols)
        grid.cells[row][col] = !grid.cells[row][col];
}

// Render the grid
inline void renderGrid(SDL_Renderer *renderer, const Grid &grid)
{
    // Draw cells
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

    // Draw grid lines only if cells are large enough
    if (grid.cellSize >= 4)
    {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 200); // White for grid lines
        for (int x = 0; x <= grid.cols * grid.cellSize; x += grid.cellSize)
            SDL_RenderDrawLine(renderer, x + grid.offsetX, 0 + grid.offsetY,
                               x + grid.offsetX, grid.rows * grid.cellSize + grid.offsetY);

        for (int y = 0; y <= grid.rows * grid.cellSize; y += grid.cellSize)
            SDL_RenderDrawLine(renderer, 0 + grid.offsetX, y + grid.offsetY,
                               grid.cols * grid.cellSize + grid.offsetX, y + grid.offsetY);
    }

    // Draw center marker
    if (grid.cellSize >= 4)
    {
        int centerX = (grid.cols / 2) * grid.cellSize + grid.offsetX;
        int centerY = (grid.rows / 2) * grid.cellSize + grid.offsetY;
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for center marker
        SDL_RenderDrawLine(renderer, centerX - 5, centerY, centerX + 5, centerY);
        SDL_RenderDrawLine(renderer, centerX, centerY - 5, centerX, centerY + 5);
    }
}

// Handle mouse and touch events for pre-game (placing cells + panning)
inline void handlePreGameEvent(SDL_Event &event, Grid &grid, InputState &input, SDL_Window *window, bool allowToggle)
{
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
            int w, h;
            SDL_GetWindowSize(window, &w, &h);
            panGrid(grid, event.motion.x - input.lastMouseX, event.motion.y - input.lastMouseY, w, h);
            input.lastMouseX = event.motion.x;
            input.lastMouseY = event.motion.y;
        }
        break;
    case SDL_FINGERDOWN:
        // Reset gesture flag on the first finger of a new interaction
        if (SDL_GetNumTouchFingers(event.tfinger.touchId) == 1)
        {
            input.inMultiGesture = false;
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
        break;
    case SDL_FINGERMOTION:
        if (input.fingerDown && !input.inMultiGesture)
        {
            if (!input.isPanning)
            {
                int w, h;
                SDL_GetWindowSize(window, &w, &h);
                // Convert normalized finger movement to pixel distance
                float dx_pixels = (event.tfinger.x - input.startFingerX) * w;
                float dy_pixels = (event.tfinger.y - input.startFingerY) * h;

                // Define threshold as a fraction of the cell size, making it dynamic
                float pixel_threshold = grid.cellSize * 0.4f; // Move 40% of a cell to be a pan

                if (std::abs(dx_pixels) > pixel_threshold || std::abs(dy_pixels) > pixel_threshold)
                {
                    input.isPanning = true;
                }
            }

            if (input.isPanning)
            {
                int w, h;
                SDL_GetWindowSize(window, &w, &h);
                int dx = (event.tfinger.x - input.lastFingerX) * w;
                int dy = (event.tfinger.y - input.lastFingerY) * h;
                panGrid(grid, dx, dy, w, h);
                input.lastFingerX = event.tfinger.x;
                input.lastFingerY = event.tfinger.y;
            }
        }
        break;
    }
}
