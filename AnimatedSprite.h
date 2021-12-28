#ifndef ANIMATEDSPRITE_H_INCLUDED
#define ANIMATEDSPRITE_H_INCLUDED

#include <vector>
#include "Sprite.h"
#include "Animation.h"

class AnimatedSprite : public Sprite{
public:
    AnimatedSprite(const char*, SDL_Renderer*, std::vector<SDL_Rect>);
    void drawAndUpdate(SDL_Renderer*, SDL_Rect* = NULL);
    void drawAndUpdateR(SDL_Renderer*, SDL_Rect*, const double, const SDL_RendererFlip);


private:
    Animation animation;
};

#endif // ANIMATEDSPRITE_H_INCLUDED
