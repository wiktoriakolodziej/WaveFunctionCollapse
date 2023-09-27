#pragma once
#include "Texture.h"


class TextureBmp : public Texture {
	SDL_Surface* image;
public:
	TextureBmp(std::string name): Texture(){
		image = SDL_LoadBMP(name.c_str());
	}
	TextureBmp(SDL_Surface* surface) : Texture() {
		image = surface;
	}
	TextureBmp(std::string name, int* _edges) : Texture(_edges) {
		image = SDL_LoadBMP(name.c_str());
	}
	TextureBmp(SDL_Surface* surface, int* _edges) : Texture(_edges) {
		image = surface;
	}
	SDL_Surface* getImage() override {
		return image;
	}
	~TextureBmp() {
		SDL_FreeSurface(image);
	}
};
