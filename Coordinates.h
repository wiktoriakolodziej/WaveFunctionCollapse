#pragma once
#include <SDL.h>
#include <vector>

template <int height, int width>
class Coordinates {
    std::vector<std::vector<SDL_Rect>> positions;
public:
	Coordinates(int cellSize) {
        std::vector<SDL_Rect> container;
        SDL_Rect cell;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                cell.x = j * cellSize;
                cell.y = i * cellSize;
                cell.w = cellSize;
                cell.h = cellSize;
                container.push_back(cell);
            }
            positions.push_back(container);
            container.clear();
        }
	}
    std::vector<SDL_Rect>& operator[] (int m) { 
        return positions[m]; 
    }
    ~Coordinates() {

    }
};
