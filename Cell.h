#pragma once
#include "TextureManager.h"

class Cell {
	bool collapsed;
	Texture* image; 
	TextureManager* entropy;
public:
	Cell* up;
	Cell* right;
	Cell* down;
	Cell* left;

	Cell(TextureManager* tiles, Cell* up = nullptr, Cell* left = nullptr, Cell* right = nullptr, Cell* down = nullptr) :
		image(tiles->operator[](0)), up(up), right(right), down(down), left(left), collapsed(false)
	{
		entropy = new TextureManager(*tiles);
		entropy->erase(0);
	}
	void setImage(Texture* newImage) {
		image = newImage;
	}
	SDL_Surface* getImage() {
		return image->getImage();
	}
	int imageSideUp() {
		return image->operator[](0);
	}
	int imageSideRight() {
		return image->operator[](1);
	}
	int imageSideDown() {
		return image->operator[](2);
	}
	int imageSideLeft() {
		return image->operator[](3);
	}
	void collapse(Texture* newImage) {
		setImage(newImage);
		collapsed = true;
	}
	void uncollapse(TextureManager* tiles) {
		collapsed = false;
		setImage(tiles->operator[](0));
		entropy->clear();
		calculateEntropy(tiles);
	}
	int getEntropySize() {
		return entropy->size();
	}
	bool isCollapsed() {
		return collapsed;
	}
	TextureManager* getEntropy() {
		TextureManager* output = new TextureManager(*entropy);
		return output;
	}
	void calculateEntropy(TextureManager* tiles) {
		
		//entropy->clear(); 
		delete entropy; //clear last entropy
		
		if (this->collapsed == true) { //protection from calculating collapsed tiles
			entropy = new TextureManager();
			entropy->Add(image);
			return;
		}
		entropy = new TextureManager(*tiles);
		entropy->erase(0);
		int match = 0, entropySize = entropy->size();
		if (up != nullptr) {
			TextureManager* upTiles = new TextureManager(*up->getEntropy());
			int x = 0;
			while(x != entropy->size()) {
				for (int y = 0; y < up->getEntropySize(); y++) {
					if (upTiles->operator[](y)->operator[](2) == 0 || entropy->operator[](x)->operator[](0) == upTiles->operator[](y)->operator[](2))
					{
						match++;
					}
				}
				if (match == 0) {
					entropy->erase(x);
					x--;
					entropySize--;
				}
				x++;
				match = 0;
			}
			delete upTiles;
		}
		if (right != nullptr) {
			entropySize = entropy->size();
			TextureManager* rightTiles = new TextureManager(*right->getEntropy());
			for (int x = 0; x < entropySize; x++) {

				for (int y = 0; y < right->getEntropySize(); y++) {
					if (rightTiles->operator[](y)->operator[](3) == 0 || entropy->operator[](x)->operator[](1) == rightTiles->operator[](y)->operator[](3))
					{
						match++;
					}
				}
				if (match == 0) {
					entropy->erase(x);
					x--;
					entropySize--;
				}
				match = 0;

			}
			delete rightTiles;
		}
		if (down != nullptr) {
			entropySize = entropy->size();
			TextureManager* downTiles = new TextureManager(*down->getEntropy());
			for (int x = 0; x < entropySize; x++) {

				for (int y = 0; y < down->getEntropySize(); y++) {
					if (downTiles->operator[](y)->operator[](0) == 0 || entropy->operator[](x)->operator[](2) == downTiles->operator[](y)->operator[](0))
					{
						match++;
					}
				}
				if (match == 0) {
					entropy->erase(x);
					x--;
					entropySize--;
				}
				match = 0;

			}
			delete downTiles;
		}
		if (left != nullptr) {
			entropySize = entropy->size();
			TextureManager* leftTiles = new TextureManager(*left->getEntropy());
			for (int x = 0; x < entropySize; x++) {

				for (int y = 0; y < left->getEntropySize(); y++) {
					if (leftTiles->operator[](y)->operator[](1) == 0 || entropy->operator[](x)->operator[](3) == leftTiles->operator[](y)->operator[](1))
					{
						match++;
					}
				}
				if (match == 0) {
					entropy->erase(x);
					x--;
					entropySize--;
				}
				match = 0;

			}
			delete leftTiles;
		}


		/*for (int x = 1; x < tiles->size(); x++) {
			if ((up == nullptr || (up->imageSideDown() == 0 || tiles->operator[](x)->operator[](0) == up->imageSideDown())) &&
				(right == nullptr || (right->imageSideLeft() == 0 || tiles->operator[](x)->operator[](1) == right->imageSideLeft())) &&
				(down == nullptr || (down->imageSideUp() == 0 || tiles->operator[](x)->operator[](2) == down->imageSideUp())) &&
				(left == nullptr || (left->imageSideRight() == 0 || tiles->operator[](x)->operator[](3) == left->imageSideRight())))
			{
				entropy->Add(tiles->operator[](x));
			}
			
		}*/
	}
	~Cell() {
		delete entropy;
	}

};
