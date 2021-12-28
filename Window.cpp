#include "Window.h"

Window::Window(const char* title, int x, int y, int w, int h, Uint32 flags){
    pWindow = SDL_CreateWindow(title, x, y, w, h, flags);
    if(pWindow == NULL){
        SDL_Log("Can't init window: %s", SDL_GetError());
    }else{
        windowIsOpen = true;
        resizeWindow(w, h);
    }
    windowIsFullscreen = false;
}

Window::Window(){
    pWindow = NULL;
    windowIsOpen = false;
}

void Window::createWindow(const char* title, int x, int y, int w, int h, Uint32 flags){
    pWindow = SDL_CreateWindow(title, x, y, w, h, flags);
    if(pWindow == NULL){
        SDL_Log("Can't init window: %s", SDL_GetError());
    }else{
        windowIsOpen = true;
        resizeWindow(w, h);
    }
}

const int& Window::getWidth(){
    return width;
}

const int& Window::getHeight(){
    return height;
}

void Window::resizeWindow(const int& newWidth, const int& newHeight){
    width = newWidth;
    height = newHeight;
}

void Window::enableFullscreen(){
    SDL_SetWindowFullscreen(pWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
    windowIsFullscreen = true;
}

void Window::disableFullscreen(){
    SDL_SetWindowFullscreen(pWindow, 0);
    windowIsFullscreen = false;
}

SDL_Window* Window::getPtr(){
    return pWindow;
}

bool Window::isOpen(){
    return windowIsOpen;
}

bool Window::isFullscreen(){
    return windowIsFullscreen;
}

void Window::close(){
    SDL_DestroyWindow(pWindow);
    windowIsOpen = false;
}

Window::~Window(){
    close();
}
