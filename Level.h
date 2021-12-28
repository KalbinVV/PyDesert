#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include <string>
#include <vector>
#include <SDL.h>
#include "Sprite.h"
#include "LevelObject.h"
#include "InterpreterInfo.h"

class Level{
public:
    Level(std::string);
    void load(SDL_Renderer*, int, int);
    void loadObjects(SDL_Renderer*, int, int);
    void draw(SDL_Renderer*);
    const int& getTileWidth();
    const int& getTileHeight();
    std::vector<LevelObject>& getObjects();
    const std::string& getRequiredOutput();
    const std::string& getDescription();
    InterpreterInfo& getInterpreterInfo();
    bool isCompleted();
    void setCompleted();
    ~Level();

private:
    std::string path;
    std::string requiredOutput;
    std::string description;
    InterpreterInfo interpreterInfo;
    int levelTileWidth;
    int levelTileHeight;
    SDL_Texture* savedBackground;
    std::vector<LevelObject> objects;
    bool completed;
};

#endif // LEVEL_H_INCLUDED
