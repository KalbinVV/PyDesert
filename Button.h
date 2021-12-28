#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

#include <SDL.h>
#include <SDL_ttf.h>
#include <functional>
#include <string>
#include "Sprite.h"

class Button{
public:
    Button(float, float, Sprite*, std::function<void()>, std::string = "");
    SDL_Rect getRect(const int, const int, const int);
    void draw(SDL_Renderer*, TTF_Font*, const int, const int, const int);
    void run();

private:
    float x;
    float y;
    Sprite* sprite;
    std::string textAfterButton;
    std::function<void()> func;
};

#endif // BUTTON_H_INCLUDED
