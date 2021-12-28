#include <stdexcept>
#include "Sprite.h"
#include "SDL_image.h"

Sprite::Sprite(const char* path, SDL_Renderer* renderer){
    SDL_Surface* surface =  IMG_Load(path);
    if(surface == NULL){
        SDL_Log("Can't load image:%s", path);
        throw std::runtime_error("Can't load image!");
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if(texture == NULL){
        SDL_Log("Can't create texture for sprite: %s.\nError:%s", path, SDL_GetError());
    }
    SDL_FreeSurface(surface);
}

void Sprite::draw(SDL_Renderer* renderer, SDL_Rect* srcRect, SDL_Rect* dstRect){
    SDL_RenderCopy(renderer, texture, srcRect, dstRect);
}

SDL_Texture* Sprite::getTexture(){
    return texture;
}

Sprite::~Sprite(){
    SDL_DestroyTexture(texture);
}
