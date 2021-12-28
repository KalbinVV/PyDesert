#ifndef LEVELOBJECT_H_INCLUDED
#define LEVELOBJECT_H_INCLUDED

#include <string>
#include "Sprite.h"

enum class ObjectType{
    Description,
    DescriptionNoSee,
    Terminal
};

class LevelObject{
public:
    LevelObject(std::string, ObjectType, int, int, SDL_Rect, std::string);
    ObjectType getType();
    void draw(SDL_Renderer*, int, int);
    const int& getX();
    const int& getY();
    std::string getValue();

private:
    ObjectType type;
    std::string tileSetPath;
    std::string value;
    int x;
    int y;
    SDL_Rect srcRect;
};

#endif // LEVELOBJECT_H_INCLUDED
