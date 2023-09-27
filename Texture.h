#pragma once
#include <string>
#include <SDL.h>
#include "Edges.h"


class Texture {
	Edges* edges;
public:
	Texture() {
		edges = new Edges();
	}
	Texture(int* _edges) {
		edges = new Edges(_edges);
	}
	int operator[](int position) {
		if (position < 4)
			return edges->operator[](position);
	}
	bool areEdgesSet() {
		return edges->operator[](0) != -1;
	}
	void changeEdges(int up, int right, int down, int left) {
		edges->changeEdges(up, right, down, left);
	}
	virtual SDL_Surface* getImage() = 0;
	/*Tile* rotate() {
		SDL_Surface* _image = SDL_ConvertSurface(image, image->format, 0);
		int edges[4] = { this->edges[3], this->edges[0], this->edges[1], this->edges[2] };
		Tile newTile(_image, edges);
		return &newTile;
	}*/
	virtual ~Texture(){
		delete edges;
	}

};
