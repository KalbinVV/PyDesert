#ifndef SPRITE_H_INCLUDED
#define SPRITE_H_INCLUDED

#include <SDL.h>

class Sprite{
public:
    Sprite(const char*, SDL_Renderer*);
    void draw(SDL_Renderer*, SDL_Rect* = NULL, SDL_Rect* = NULL);
    SDL_Texture* getTexture();
    ~Sprite();
private:
    SDL_Texture* texture;
};

#endif // SPRITE_H_INCLUDED
