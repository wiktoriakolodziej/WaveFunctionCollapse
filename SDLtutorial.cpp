#include <filesystem>
#include <regex>

#include "Grid.h"
#include "FileExtensionException.h"

const int width = 30;
const int height = 15;
const int cellSize = 64;

TextureManager* readFromDirectory(std::filesystem::path& path) {
    TextureManager* autoTiles = new TextureManager;
    std::filesystem::path nestedPath;
    std::string filename;
    std::regex bmpRegex(".+\\.bmp"), pngRegex(".+\\.png");
    for (const std::filesystem::directory_entry& dir_entry : std::filesystem::directory_iterator(path)) {
        nestedPath = dir_entry.path().filename();
        filename = path.string() + "/" + nestedPath.string();
        Texture* tile = nullptr;
        if (std::regex_search(filename, bmpRegex)) {
            tile = new TextureBmp(filename);
        }
        else if (std::regex_search(filename, pngRegex)) {
            tile = new TexturePng(filename);
        }
        else {
            delete autoTiles;
            throw FileExtensionException();
        }
        autoTiles->Add(tile);
    }
    return autoTiles;
}
bool message() {
    std::cout << "would you like to try with another directory? ";
    std::string condition;
    std::cin >> condition;
    system("cls");
    if (condition == "yes") {
        return true;
    }
    else {
        return false;
    }
}
bool UserInterface(TextureManager** textures, std::string& saveFile) {
    //pointer to a pointer so it won't change the copy of the pointer
    std::string directory = "special";
    bool x = true;
    while (x) {
        try {
            x = false;

            //let user give directory name and check if it exists
        /*    std::cout << "write a directory with your tiles: ";
            std::cin >> directory;*/
            if (!std::filesystem::exists(directory)) {
                std::cout << "given directory doesn't exist" << std::endl;
                x = message();
                if (x == false)
                    return false;
                continue;
            }
            system("cls");

            //read tiles from the directory given by an user
            std::filesystem::path path(directory);
            *textures = readFromDirectory(path);
        }
        //exception in case if any file in given directory is invalid
        catch (FileExtensionException exc) {

            std::cout << exc.what() << std::endl;
            x = message();
            if (x == false)
                return false;
        }
    }

    //let user decide if he wants to save their work
    std::string condition = "no";
    //std::cout << "would yo like to save your work? ";
    //std::cin >> condition;

    if (condition == "yes") {
        std::cout << "how would like to name it? ";
        std::cin >> saveFile;
        saveFile += ".png";
    }
    return true;
}
void mainLoop(SDL_Window** window, SDL_Surface** windowSurface, std::mutex* windowMutex, std::string& saveFile) {
    SDL_Init(SDL_INIT_VIDEO);   //initialize SDL

    //create a window
    *window = SDL_CreateWindow("name", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width * cellSize, height * cellSize, SDL_WINDOW_SHOWN /*|*/);
    //create a window surface attached to the window
    *windowSurface = SDL_GetWindowSurface(*window);


    //main loop with managing poll events
    bool isRunning = true;
    SDL_Event ev;

    while (isRunning) {
        while (SDL_PollEvent(&ev) != 0) {
            if (ev.type == SDL_QUIT) isRunning = false;
            if (ev.type == SDL_KEYDOWN) {
                switch (ev.key.keysym.sym) {
                case SDLK_ESCAPE:
                    isRunning = false;
                }
            }
            if (ev.type == SDL_WINDOWEVENT_MINIMIZED) SDL_MinimizeWindow(*window);

            /*else if (ev.type == SDL_KEYDOWN) {
                switch (ev.key.keysym.sym) {
                case SDLK_1:
                    currentSurface = image1;
                    break;
                case SDLK_2:
                    currentSurface = image2;
                    break;
                case SDLK_3 :
                    currentSurface = image3;
                    break;
                }
            }*/
        }
    }

    //lock on mutex so it won't be used in a gid.draw() method
    std::lock_guard<std::mutex> lock(*windowMutex);

    //save work if requested
    if (!saveFile.empty())
        IMG_SavePNG(*windowSurface, saveFile.c_str());

    //free the surface from the heap
    //destroys window but also window surface
    SDL_DestroyWindow(*window);

    //set both to the nullptr 
    *windowSurface = nullptr;
    *window = nullptr;
    SDL_Quit();


  
}



int main(int argc, char* argv[])
{

    //create a window
    SDL_Window* window = nullptr;
    //create a window surface attached to the window
    SDL_Surface* windowSurface = nullptr;

    //create a mutex to access the window surface
    std::mutex* windowMutex = new std::mutex;

    //collect data from user
    TextureManager* autoTiles = nullptr;
    std::string saveFile;
    if (!UserInterface(&autoTiles, saveFile)) return 0;


    //thread managing window
    std::thread mainLoopThread(mainLoop, &window, &windowSurface, windowMutex, std::ref(saveFile));
    mainLoopThread.detach();


    //create a grid of coordinates
    Coordinates<height, width>* coordinates = new Coordinates<height, width>(cellSize);
    //create a grid of surfaces
    Grid<height, width>* grid = new Grid<height, width>(autoTiles);

    //run filling of the grid in a separate thread
    std::thread calculations = grid->fillThread(autoTiles, windowSurface, coordinates, &window, windowMutex);
    calculations.join();
   
 //save work if requested
    if (!saveFile.empty())
        IMG_SavePNG(windowSurface, saveFile.c_str());

    //free the surface from the heap
    //destroys window but also window surface
    SDL_DestroyWindow(window);

    //set both to the nullptr 
    windowSurface = nullptr;
    window = nullptr;
    SDL_Quit();
    
    //free the memory from the heap
    delete grid;
    delete coordinates;
    delete autoTiles;
    delete windowMutex;

    return 0;
}
