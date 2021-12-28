#include "Animation.h"

Animation::Animation(Sprite* sprite_, std::vector<SDL_Rect> rects_){
    sprite = sprite_;
    rects = rects_;
    itRect = rects.cbegin();
}

void Animation::update(){
    itRect++;
    if(itRect == rects.cend()){
        itRect = rects.cbegin();
    }
}

void Animation::draw(SDL_Renderer* renderer, SDL_Rect* dstRect){
    SDL_RenderCopy(renderer, sprite->getTexture(), &(*itRect), dstRect);
}

void Animation::drawR(SDL_Renderer* renderer, SDL_Rect* dstRect, const double angle, const SDL_RendererFlip flip){
    SDL_RenderCopyEx(renderer, sprite->getTexture(), &(*itRect), dstRect, angle, NULL, flip);
}
