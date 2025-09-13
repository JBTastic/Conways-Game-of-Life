#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cmath> // for std::abs

int main(int argc, char *argv[])
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not be initialized! SDL_Error: "
                  << SDL_GetError() << std::endl;
        return 1;
    }

    // Create a resizable window
    SDL_Window *window = SDL_CreateWindow(
        "Conways Game of Life",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        std::cerr << "Window could not be created! SDL_Error: "
                  << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << "Renderer could not be created! SDL_Error: "
                  << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Cell size in pixels
    const int cellSize = 30;

    // Logical grid
    const int cols = 160;
    const int rows = 90;
    std::vector<std::vector<bool>> grid(rows, std::vector<bool>(cols, false));

    // Panning variables
    int offsetX = 0;
    int offsetY = 0;
    bool rightMouseDown = false;
    int lastMouseX = 0;
    int lastMouseY = 0;

    // Touch variables
    bool fingerDown = false;
    bool isPanning = false;
    float startFingerX = 0.0f;
    float startFingerY = 0.0f;
    float lastFingerX = 0.0f;
    float lastFingerY = 0.0f;

    bool running = true;
    SDL_Event event;

    const float PAN_THRESHOLD = 0.005f; // fraction of window size

    // Main loop
    while (running)
    {
        // Poll events
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_MOUSEBUTTONDOWN:

                // On left click toggle cell state
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    int mouseX = event.button.x - offsetX;
                    int mouseY = event.button.y - offsetY;

                    int col = mouseX / cellSize;
                    int row = mouseY / cellSize;

                    if (row >= 0 && row < rows && col >= 0 && col < cols)
                        grid[row][col] = !grid[row][col];
                }

                // On right click start panning
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    rightMouseDown = true;
                    lastMouseX = event.button.x;
                    lastMouseY = event.button.y;
                }
                break;

            // Stop panning
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_RIGHT)
                    rightMouseDown = false;
                break;

            // Panning logic
            case SDL_MOUSEMOTION:
                if (rightMouseDown)
                {
                    int dx = event.motion.x - lastMouseX;
                    int dy = event.motion.y - lastMouseY;
                    offsetX += dx;
                    offsetY += dy;
                    lastMouseX = event.motion.x;
                    lastMouseY = event.motion.y;
                }
                break;

            // Touchpad two-finger scroll support
            case SDL_MOUSEWHEEL:
                offsetX -= event.wheel.x * (cellSize / 2);
                offsetY += event.wheel.y * (cellSize / 2);
                break;

            // Touch input
            case SDL_FINGERDOWN:
                fingerDown = true;
                isPanning = false;
                startFingerX = event.tfinger.x;
                startFingerY = event.tfinger.y;
                lastFingerX = startFingerX;
                lastFingerY = startFingerY;
                break;

            case SDL_FINGERUP:
                // If finger didn't move much, toggle cell
                if (!isPanning)
                {
                    int w, h;
                    SDL_GetWindowSize(window, &w, &h);
                    int touchX = startFingerX * w - offsetX;
                    int touchY = startFingerY * h - offsetY;
                    int col = touchX / cellSize;
                    int row = touchY / cellSize;
                    if (row >= 0 && row < rows && col >= 0 && col < cols)
                        grid[row][col] = !grid[row][col];
                }
                fingerDown = false;
                isPanning = false;
                break;

            case SDL_FINGERMOTION:
                if (fingerDown)
                {
                    float dxf = event.tfinger.x - startFingerX;
                    float dyf = event.tfinger.y - startFingerY;
                    if (!isPanning)
                    {
                        if (std::abs(dxf) > PAN_THRESHOLD || std::abs(dyf) > PAN_THRESHOLD)
                            isPanning = true; // start panning
                    }

                    if (isPanning)
                    {
                        int w, h;
                        SDL_GetWindowSize(window, &w, &h);
                        int dx = (event.tfinger.x - lastFingerX) * w;
                        int dy = (event.tfinger.y - lastFingerY) * h;
                        offsetX += dx;
                        offsetY += dy;
                        lastFingerX = event.tfinger.x;
                        lastFingerY = event.tfinger.y;
                    }
                }
                break;
            }
        }

        // Get current window size for dynamic grid rendering
        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        // Clear screen with black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw alive cells in white
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int row = 0; row < rows; row++)
        {
            for (int col = 0; col < cols; col++)
            {
                if (grid[row][col])
                {
                    SDL_Rect cellRect = {col * cellSize + offsetX, row * cellSize + offsetY, cellSize, cellSize};
                    SDL_RenderFillRect(renderer, &cellRect);
                }
            }
        }

        // Vertical lines
        for (int x = 0; x <= cols * cellSize; x += cellSize)
        {
            SDL_RenderDrawLine(renderer, x + offsetX, 0 + offsetY, x + offsetX, rows * cellSize + offsetY);
        }

        // Horizontal lines
        for (int y = 0; y <= rows * cellSize; y += cellSize)
        {
            SDL_RenderDrawLine(renderer, 0 + offsetX, y + offsetY, cols * cellSize + offsetX, y + offsetY);
        }

        // Present frame
        SDL_RenderPresent(renderer);
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
