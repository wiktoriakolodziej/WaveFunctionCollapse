#pragma once
#include "Texture.h"

#include <SDl_image.h>


class TexturePng : public Texture {
	SDL_Surface* image;

public:
	TexturePng(std::string name) : Texture() {
		image = IMG_Load(name.c_str());
	}
	TexturePng(SDL_Surface* surface) : Texture() {
		image = surface;
	}
	TexturePng(std::string name, int* _edges) : Texture(_edges) {
		image = IMG_Load(name.c_str());
	}
	TexturePng(SDL_Surface* surface, int* edges) : Texture(edges) {
		image = surface;
	}
	SDL_Surface* getImage() override {
		return image;
	}
	~TexturePng() {
		SDL_FreeSurface(image);
	}
};
