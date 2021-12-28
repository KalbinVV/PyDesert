#include "Button.h"
#include "Text.h"

Button::Button(float x_, float y_, Sprite* sprite_, std::function<void()> func_, std::string textAfterButton_){
    x = x_;
    y = y_;
    sprite = sprite_;
    textAfterButton = textAfterButton_;
    func = func_;
}

SDL_Rect Button::getRect(const int windowWidth, const int windowHeight, const int BUTTON_SIZE){
    SDL_Rect rect{
        x: static_cast<int>(x * windowWidth),
        y: static_cast<int>(y * windowHeight),
        w: BUTTON_SIZE,
        h: BUTTON_SIZE
    };
    if(!textAfterButton.empty()){
        rect.w = rect.w + static_cast<int>(BUTTON_SIZE / 5 * textAfterButton.length());
    }
    return rect;
}

void Button::draw(SDL_Renderer* renderer, TTF_Font* font, const int windowWidth, const int windowHeight, const int BUTTON_SIZE){
    SDL_Rect rect{
        x: static_cast<int>(x * windowWidth),
        y: static_cast<int>(y * windowHeight),
        w: BUTTON_SIZE,
        h: BUTTON_SIZE
    };
    sprite->draw(renderer, NULL, &rect);
    if(!textAfterButton.empty()){
        Text text(textAfterButton, font);
        rect.w = static_cast<int>(BUTTON_SIZE / 5 * text.getString().length());
        rect.x = rect.x + BUTTON_SIZE + 2;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        text.drawSolid(renderer, &rect, {255, 255, 255});
    }
}

void Button::run(){
    func();
}
