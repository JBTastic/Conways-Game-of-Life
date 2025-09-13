#pragma once
#include "pre_game.h"

// Update the grid based on Conway's Game of Life rules
inline void updateGrid(Grid &grid)
{
    if (grid.rows == 0 || grid.cols == 0) return;

    std::vector<std::vector<bool>> newCells = grid.cells;

    for (int row = 0; row < grid.rows; ++row)
    {
        for (int col = 0; col < grid.cols; ++col)
        {
            int liveNeighbors = 0;
            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    if (i == 0 && j == 0) continue;

                    int neighbor_row = row + i;
                    int neighbor_col = col + j;

                    if (neighbor_row >= 0 && neighbor_row < grid.rows &&
                        neighbor_col >= 0 && neighbor_col < grid.cols &&
                        grid.cells[neighbor_row][neighbor_col])
                    {
                        liveNeighbors++;
                    }
                }
            }

            if (grid.cells[row][col] && (liveNeighbors < 2 || liveNeighbors > 3))
            {
                newCells[row][col] = false;
            }
            else if (!grid.cells[row][col] && liveNeighbors == 3)
            {
                newCells[row][col] = true;
            }
        }
    }

    grid.cells = newCells;
}
