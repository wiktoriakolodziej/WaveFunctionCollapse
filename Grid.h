#pragma once


//#include <algorithm>
#include <random> //random numbers

#include <thread>
#include <mutex>

#include "Cell.h"
#include "Coordinates.h"



std::random_device seed;
std::mt19937 mt(seed());
std::uniform_int_distribution<> dist(0, 100000);

template <int height, int width>
class Grid {
	Cell* grid[height][width];

public:
	Grid(TextureManager* textures) {

		//create the cells
		for (int x = 0; x < height; x++) {
			for (int y = 0; y < width; y++) {
				grid[x][y] = new Cell(textures);
			}
		}

		//set  pointers to the neighbours of each cell
		for (int x = 0; x < height; x++) {
			for (int y = 0; y < width; y++) {
				if (x != 0)
					grid[x][y]->up = grid[x - 1][y];
				if (y != width - 1)
					grid[x][y]->right = grid[x][y + 1];
				if (x != height - 1)
					grid[x][y]->down = grid[x + 1][y];
				if (y != 0)
					grid[x][y]->left = grid[x][y - 1];
			}
		}
	}
	/*Cell* operator[](std::pair<int, int> indexes) {
		return grid[indexes.first][indexes.second];
	}*/
	bool fill(TextureManager* textures, SDL_Surface* windowSurface, Coordinates<height, width>* coords, SDL_Window** window, std::mutex* mut);
	std::thread fillThread(TextureManager* textures, SDL_Surface* windowSurface, Coordinates<height, width>* coords, SDL_Window** window, std::mutex* mut) {
		return std::thread([this, textures, windowSurface, coords, window, mut] { fill(textures, windowSurface, coords, window, mut); });
		}
	bool check(Cell* cell, TextureManager* tiles);
	void draw( SDL_Surface* windowSurface, Coordinates<height, width>* coords, SDL_Window** window, std::mutex* mut);
	~Grid() {
		for (int x = 0; x < height; x++) {
			for (int y = 0; y < width; y++) {
				delete grid[x][y];
			}
		}
	}
};

//recursively fills the grid with matching surfaces
template<int height, int width>
bool Grid<height, width>::fill(TextureManager* textures, SDL_Surface* windowSurface, Coordinates<height, width>* coords, SDL_Window** window, std::mutex* mut)
{
	if (*window == nullptr) //not a good solution
		return false;
	/*
	* find cells with least entropy and add them to a new vector
	* choose one randomly
	* LOOP: choose randomly from it's options of tiles
	* call check on it until it returns true (change tile everytime it returns false)
	* if cell has 0 options of tiles return false
	* when check returns true change cell's image and draw grid
	* call fill if it return true return true
	* else delete this tile from options and LOOP again
	*/
	
	
	std::vector<Cell*> bestCells;	//cells with least entropy
	
	int leastEntropy = textures->size();	//set least entropy to max + 1
	
	int currentCellEntropy;	//variable to hold entropy of current cell

	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			if (!grid[x][y]->isCollapsed()) {
				currentCellEntropy = grid[x][y]->getEntropySize();	//get size of entropy

				if (currentCellEntropy < leastEntropy) {	//check if it's new least entropy
					leastEntropy = currentCellEntropy;
					bestCells.clear();
				}

				if (currentCellEntropy == leastEntropy)	//if cell has min entropy add it to the best cells
					bestCells.push_back(grid[x][y]);
			}
		}
	}
	if (leastEntropy == 0)	//chech if any tile has no more options of surfaces
		return false;
	else if (leastEntropy == textures->size())	//chech if grid is completed
		return true;
	
	int cellIndex = dist(mt) % bestCells.size();	//choose one of the cells with least entropy randomly
	Cell* chosenCell = bestCells[cellIndex];


	TextureManager* tileOptions = chosenCell->getEntropy();	//get the entropy of the chosen tile
	int tileIndex;
	Texture* chosenTile = nullptr;

	
	while (true) {
		tileIndex = dist(mt) % tileOptions->size();	//choose one of its available surfaces randomly
		chosenTile = tileOptions->operator[](tileIndex);
		

		chosenCell->collapse(chosenTile);		//collapse it with chosen tile
		if (check(chosenCell, textures)) {
			draw(windowSurface, coords, window, mut);	//draw a grid
			fill(textures, windowSurface, coords, window, mut);
			delete tileOptions;
			return true;


			//if (fill(textures, windowSurface, coords, window, mut)) {	//if fill returns true that means that grid is full
			//	delete tileOptions;
			//	return true;
			//}

			//else {		//if fill returns false some tile has 0 entropy 
			//	tileOptions->erase(tileIndex);	//erase this surface from available options as it leads to blockade

			//	if (tileOptions->size() == 0) {	//if current tile has 0 available opt. that means it has 0 entropy
			//		delete tileOptions;
			//		chosenCell->uncollapse(textures);	//uncollapse it
			//		return false;
			//	}
			//}
		}
		else {		//if check returns false some tile has 0 entropy 

			
			tileOptions->erase(tileIndex);	//erase this surface from available options as it leads to blockade

			if (tileOptions->size() == 0) {	//if current tile has 0 available opt. that means it has 0 entropy
				delete tileOptions;
				chosenCell->uncollapse(textures);	//uncollapse it
				return false;
			}
		}
		
	}
	
}

//calculate new entropy of tiles for which it has changed
template<int height, int width>
bool Grid<height, width>::check(Cell* cell, TextureManager* tiles)
{
	int lastEntropy = cell->getEntropySize();
	cell->calculateEntropy(tiles);
	int currentEntropy = cell->getEntropySize();
	if (currentEntropy == 0)
		return false;
	if (lastEntropy != currentEntropy) {
		if (cell->up != nullptr && !check(cell->up, tiles))
			return false;
		if (cell->right != nullptr && !check(cell->right, tiles))
			return false;
		if (cell->down != nullptr && !check(cell->down, tiles))
			return false;
		if (cell->left != nullptr && !check(cell->left, tiles))
			return false;
	}
	return true;
	
}

//draws an image
template<int height, int width>
void Grid<height, width>::draw(SDL_Surface* windowSurface, Coordinates<height, width>* coords, SDL_Window** window, std::mutex* mut)
{
	//lock on mutex so the window won't be destoyed during the drawing
	std::lock_guard<std::mutex> lock(*mut);
	if (*window != nullptr) {
		
		for (int x = 0; x < height; x++) {
			for (int y = 0; y < width; y++) {
				//draw surfaces on the window surface
				SDL_BlitSurface(grid[x][y]->getImage(), NULL, windowSurface, &((*coords)[x][y]));
			}
		}
		SDL_UpdateWindowSurface(*window);
		
	}
}


