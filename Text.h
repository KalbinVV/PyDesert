#ifndef TEXT_H_INCLUDED
#define TEXT_H_INCLUDED

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

class Text{
public:
    Text(std::string, TTF_Font*);
    void setString(std::string);
    std::string getString();
    void drawSolid(SDL_Renderer*, SDL_Rect* dstRect, SDL_Color color);
    void drawBlended(SDL_Renderer*, SDL_Rect* dstRect, SDL_Color color);

private:
    std::string value;
    TTF_Font* font;
};

#endif // TEXT_H_INCLUDED
