#include "LevelObject.h"

LevelObject::LevelObject(std::string path_, ObjectType type_, int x_, int y_, SDL_Rect srcRect_, std::string value_){
    type = type_;
    x = x_;
    y = y_;
    srcRect = srcRect_;
    tileSetPath = path_;
    value = value_;
}

ObjectType LevelObject::getType(){
    return type;
}

void LevelObject::draw(SDL_Renderer* renderer, int tileWidth, int tileHeight){
    SDL_Rect dstRect{
        x: x,
        y: y,
        w: tileWidth,
        h: tileHeight
    };
    Sprite sprite(tileSetPath.c_str(), renderer);
    sprite.draw(renderer, &srcRect, &dstRect);
}

const int& LevelObject::getX(){
    return x;
}

const int& LevelObject::getY(){
    return y;
}

std::string LevelObject::getValue(){
    return value;
}
