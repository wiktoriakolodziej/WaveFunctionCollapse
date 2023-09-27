#pragma once
#include <vector>
#include <iostream>

#include "TextureBmp.h"
#include "TexturePng.h"

class TextureManager {
    std::vector<Texture*> tiles;
    std::vector<std::vector<std::string>> colors;
    int findRowColors(int start, int jump, int end, SDL_Surface* surface) {

        SDL_PixelFormat* format = surface->format;
        Uint8 bpp = surface->format->BytesPerPixel;
        Uint32 pixel;
        Uint8* pPixel;


        Uint32 temp;
        Uint8 red, green, blue;

        //rgb values in one string
        std::string color;
        //colors of all pixels in a row
        std::vector<std::string> side;

        

        for (int x = start; x < end; x += jump) {
           
            pPixel = (Uint8*)surface->pixels + x / surface->w * surface->pitch + x % surface->w * bpp;
            pixel = *(Uint32*)pPixel;

            /* Get Red component */
            temp = pixel & format->Rmask;  /* Isolate red component */
            temp = temp >> format->Rshift; /* Shift it down to 8-bit */
            temp = temp << format->Rloss;  /* Expand to a full 8-bit number */
            red = (Uint8)temp;

            /* Get Green component */
            temp = pixel & format->Gmask;  /* Isolate green component */
            temp = temp >> format->Gshift; /* Shift it down to 8-bit */
            temp = temp << format->Gloss;  /* Expand to a full 8-bit number */
            green = (Uint8)temp;

            /* Get Blue component */
            temp = pixel & format->Bmask;  /* Isolate blue component */
            temp = temp >> format->Bshift; /* Shift it down to 8-bit */
            temp = temp << format->Bloss;  /* Expand to a full 8-bit number */
            blue = (Uint8)temp;


            //check if tile is "empty"
            if ((int)red == 195 && (int)green == 195 && (int)blue == 195)
                return 0;

            color = std::to_string(red) + std::to_string(green) + std::to_string(blue);
            side.push_back(color);
            color.clear();
            
        }
        for (int x = 0; x < colors.size(); x++) {
            if (colors[x] == side) {
                return x +1; //+1 is nessesary bc 0 is reserved for blank tile
            }
        }
        colors.push_back(side);
        return colors.size();
    }
public:
    TextureManager() {
    }
    TextureManager(TextureManager& toCopy) {
        for (int x = 0; x < toCopy.size(); x++) {
            tiles.push_back(toCopy[x]);
        }
        for (int x = 0; x < toCopy.colorSize(); x++) {
            colors.push_back(toCopy.getColor(x));
        }
    }
    Texture* operator[](int position) {
        return tiles[position];
    }
    int size() {
        return tiles.size();
    }
    int colorSize() {
        return colors.size();
    }
    std::vector<std::string> getColor(int pos) {
        return colors[pos];
    }
    void erase(int index) {
        tiles.erase(tiles.begin() + index);
    }
    void clear() {
        tiles.clear();
        colors.clear();
    }
	void Add(Texture* tile) {
        if (!tile->areEdgesSet()) {
            SDL_Surface* surface = tile->getImage();
            int up = findRowColors(0, 1, surface->w, surface);
            int right = findRowColors(surface->w - 1, surface->w, surface->w * surface->h, surface);
            int down = findRowColors(surface->h * surface->w - surface->w, 1, surface->w * surface->h, surface);
            int left = findRowColors(0, surface->w, surface->h * surface->w - surface->w + 1, surface);
            tile->changeEdges(up, right, down, left);
        }
        tiles.push_back(tile);
	}
};
