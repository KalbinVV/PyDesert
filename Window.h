#ifndef WINDOW_H_INCLUDED
#define WINDOW_H_INCLUDED

#include <SDL.h>

class Window{
public:
    Window(const char*, int, int, int, int, Uint32);
    Window();
    void createWindow(const char*, int, int, int, int, Uint32);
    const int& getWidth();
    const int& getHeight();
    void resizeWindow(const int&, const int&);
    void enableFullscreen();
    void disableFullscreen();
    SDL_Window* getPtr();
    bool isOpen();
    bool isFullscreen();
    void close();
    ~Window();

private:
    bool windowIsOpen;
    bool windowIsFullscreen;
    int width;
    int height;
    SDL_Window* pWindow;
};

#endif // WINDOW_H_INCLUDED
