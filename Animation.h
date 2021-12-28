#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED

#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include "Sprite.h"

class Animation{
public:
    Animation(Sprite*, std::vector<SDL_Rect>);
    void update();
    void draw(SDL_Renderer*, SDL_Rect* = NULL);
    void drawR(SDL_Renderer*, SDL_Rect*, const double, const SDL_RendererFlip);

private:
    Sprite* sprite;
    std::vector<SDL_Rect> rects;
    std::vector<SDL_Rect>::const_iterator itRect;
};

#endif // ANIMATION_H_INCLUDED
