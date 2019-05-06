#ifndef ANIMATEDPATH_H
#define ANIMATEDPATH_H
#include <string>
#include <vector>
#include "SDL2/SDL.h"

/*
This class creates an path from an SVG file using a seperate brush.png,
and combines them into a 30 frame texture animation.
*/

typedef struct Coords{
    double x;
    double y;
} Coords;

typedef struct ProtoPath{
    std::vector<Coords> in;
    std::vector<Coords> out;
    std::vector<Coords> points;
    int colour[3];
} ProtoPath;

class AnimatedPath{
    protected:
        std::vector<SDL_Texture*> frames;
        std::vector<ProtoPath> protopaths;
        SDL_Rect dstRect;
        SDL_Renderer* renderer;
        int frameIndex;
        void loadPaths(std::string);
        void normalizePaths(int w,int h, std::vector<std::vector<Coords> > &positions);
    public:
        AnimatedPath(std::string svg, std::string brush, int animationLength, int w, int h, int xPos, int yPos, SDL_Renderer * rend);
        bool render();
};

#endif