# Conway's Game of Life

This is a C++ implementation of Conway's Game of Life, built using the SDL2 library. It features a graphical user interface with various controls for interacting with the simulation, including panning, zooming, and saving/loading grid states.

## What is Conway's Game of Life?

Conway's Game of Life is a cellular automaton devised by the British mathematician John Horton Conway in 1970. It is a zero-player game, meaning that its evolution is determined by its initial state, requiring no further input. One interacts with the Game of Life by creating an initial configuration and observing how it evolves.

The "game" is played on a 200 by 200 two-dimensional orthogonal grid of square cells, each of which is in one of two possible states, live or dead (or populated or unpopulated). Every cell interacts with its eight neighbours, which are the cells that are horizontally, vertically, or diagonally adjacent. At each step in time, the following transitions occur:

1.  Any live cell with fewer than two live neighbours dies, as if by underpopulation.
2.  Any live cell with two or three live neighbours lives on to the next generation.
3.  Any live cell with more than three live neighbours dies, as if by overpopulation.
4.  Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.

## Features

*   **Interactive Grid**: Place and remove cells in a large, scrollable grid with mouse clicks or touch input.
*   **Panning**: Navigate the grid by dragging with the right mouse button, using the mouse wheel for vertical and shift + mouse wheel for horizontal scrolling, the laptop touchpad or touch.
*   **Zooming**: Adjust the zoom level using ctrl + mouse wheel, ctrl + `+` / `-` (Keyboard), or pinch-to-zoom gestures.
*   **UI Controls**: Buttons for starting/pausing the simulation, clearing the grid, and jumping to the grid's center.
*   **Settings Menu**: Configure various aspects of the application, including mouse scroll behavior and visual aids, like showing the center of the grid.
*   **Import/Export**: Save and load grid configurations to/from text files.
*   **App Icon**: Custom application icon.

## How to Compile

To compile this project, you will need a C++17 compatible compiler (like g++ or clang++) and the SDL2 development libraries.

### Prerequisites

Make sure you have the following SDL2 development libraries installed on your system:

*   **SDL2**: The main SDL library.
*   **SDL2_ttf**: For TrueType font rendering (used for UI text).
*   **SDL2_image**: For image loading (used for the application icon).

**On Debian/Ubuntu-based systems:**
```bash
sudo apt-get update
sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev
```

**On macOS (using Homebrew):**
```bash
brew install sdl2 sdl2_ttf sdl2_image
```

**On Windows (using MSYS2/MinGW):**
Refer to the MSYS2 documentation for details.

### Compiling from Command Line

Navigate to the project's root directory in your terminal and run the following command:

```bash
g++ main.cpp ui.cpp -o gameoflife.out -lSDL2 -lSDL2_ttf -lSDL2_image
```
*(Note: On macOS, you might use `clang++` instead of `g++`.)*

Alternatively, use the provided compile.sh script:

```bash
chmod +x compile.sh
./compile.sh
```
To compile and execute in one step, use:

```bash
chmod +x compile.sh
./compile.sh -x
```

## How to Play

### Controls

*   **Placing/Removing Cells (Pre-Game State)**:
    *   **Left-Click / Single-Finger Tap**: Toggle the state of a cell (live/dead). This is only active when zoomed in enough.
*   **Panning the Grid**:
    *   **Right-Click Drag / Single-Finger Drag**: Drag the grid around.
    *   **Mouse Wheel Scroll**: Scroll the grid vertically.
    *   **Shift + Mouse Wheel Scroll**: Scroll the grid horizontally.
*   **Zooming the Grid**:
    *   **Ctrl + Mouse Wheel Scroll**: Zoom in/out.
    *   **Ctrl + `+` / `-` (Keyboard)**: Zoom in/out.
    *   **Two-Finger Pinch/Spread (Touchscreen)**: Zoom in/out.

### Buttons

*   **Settings**: Opens the settings menu. Changes to "Back" when in settings.
*   **Start / Pause**: Starts the simulation. Changes to "Pause" while running, and back to "Start" when paused.
*   **Clear**: Clears all cells on the grid (sets them to dead). Only active in the pre-game state.
*   **Center**: Jumps the camera view to the center of the entire grid.

## Settings Menu

Access the settings menu by clicking the "Settings" button.

*   **Invert Mouse Scrolling**: Toggles the direction of mouse wheel scrolling for both panning and zooming.
*   **Show Grid Center**: Toggles the visibility of a red cross marker at the center of the grid.
*   **Import Grid**:
    *   Imports a grid configuration from a text file named `grid_save.txt` in the current directory.
*   **Export Grid**:
    *   Exports the current grid state to a text file named `grid_save.txt` in the current directory.

After an import or export operation, a status message will appear at the bottom of the screen indicating success or failure.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
