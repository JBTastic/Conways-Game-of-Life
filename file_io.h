#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "pre_game.h" // For Grid struct

inline bool exportGrid(const Grid& grid, const std::string& filepath) {
    std::ofstream outFile(filepath);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filepath << std::endl;
        return false;
    }

    // Write dimensions first
    outFile << grid.rows << " " << grid.cols << std::endl;

    // Write cell data
    for (int i = 0; i < grid.rows; ++i) {
        for (int j = 0; j < grid.cols; ++j) {
            outFile << grid.cells[i][j] << (j == grid.cols - 1 ? "" : " ");
        }
        outFile << std::endl;
    }
    outFile.close();
    std::cout << "Grid exported successfully to " << filepath << std::endl;
    return true;
}

inline bool importGrid(Grid& grid, const std::string& filepath) {
    std::ifstream inFile(filepath);
    if (!inFile.is_open()) {
        std::cerr << "Error: Could not open file for reading: " << filepath << std::endl;
        return false;
    }

    int rows, cols;
    inFile >> rows >> cols;
    if (inFile.fail() || rows <= 0 || cols <= 0) {
        std::cerr << "Error: Invalid file format or dimensions in " << filepath << std::endl;
        return false;
    }

    // Resize grid and read cell data
    grid.rows = rows;
    grid.cols = cols;
    grid.cells.assign(rows, std::vector<bool>(cols, false));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int cell_val;
            inFile >> cell_val;
            if (inFile.fail()) {
                std::cerr << "Error: File format error or unexpected end of file." << std::endl;
                return false;
            }
            grid.cells[i][j] = (cell_val == 1);
        }
    }
    inFile.close();
    std::cout << "Grid imported successfully from " << filepath << std::endl;
    return true;
}
