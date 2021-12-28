#include "Text.h"

Text::Text(std::string value_, TTF_Font* font_){
    value = value_;
    font = font_;
}

void Text::setString(std::string newValue){
    value = newValue;
}

std::string Text::getString(){
    return value;
}

void Text::drawSolid(SDL_Renderer* renderer, SDL_Rect* dstRect, SDL_Color color){
    SDL_Surface* textSurface = TTF_RenderUTF8_Solid(font, value.c_str(), color);
    if(textSurface == NULL){
        SDL_Log("Error when create text surface: %s", SDL_GetError());
    }
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if(textTexture == NULL){
        SDL_Log("Error when create texture from text surface: %s", SDL_GetError());
    }
    SDL_FreeSurface(textSurface);
    SDL_RenderCopy(renderer, textTexture, NULL, dstRect);
    SDL_DestroyTexture(textTexture);
}

void Text::drawBlended(SDL_Renderer* renderer, SDL_Rect* dstRect, SDL_Color color){
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, value.c_str(), color);
    if(textSurface == NULL){
        SDL_Log("Error when create text surface: %s", SDL_GetError());
    }
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if(textTexture == NULL){
        SDL_Log("Error when create texture from text surface: %s", SDL_GetError());
    }
    SDL_FreeSurface(textSurface);
    SDL_RenderCopy(renderer, textTexture, NULL, dstRect);
    SDL_DestroyTexture(textTexture);
}
