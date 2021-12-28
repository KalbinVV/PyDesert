#include "AnimatedSprite.h"

AnimatedSprite::AnimatedSprite(const char* path, SDL_Renderer* renderer, std::vector<SDL_Rect> rects)
: Sprite(path, renderer), animation(this, rects){}
void AnimatedSprite::drawAndUpdate(SDL_Renderer* renderer, SDL_Rect* dstRect){
    animation.draw(renderer, dstRect);
    animation.update();
}

void AnimatedSprite::drawAndUpdateR(SDL_Renderer* renderer, SDL_Rect* dstRect, const double angle, const SDL_RendererFlip flip){
    animation.drawR(renderer, dstRect, angle, flip);
    animation.update();
}
