#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <vector>
#include <SDL.h>
#include "AnimatedSprite.h"

enum class PlayerState{
    Idle,
    Run
};

enum class Direction{
    Left,
    Right
};


class Player{
public:
    const int static MAX_SPEED;
    const int static DEFAULT_SPEED;

    Player();
    void setIdleSprite(const char*, SDL_Renderer*, std::vector<SDL_Rect>);
    void setRunSprite(const char*, SDL_Renderer*, std::vector<SDL_Rect>);
    PlayerState getState();
    void setState(PlayerState);
    void setDirection(Direction);
    void setSpeed(int);
    const int& getSpeed();
    const int& getX();
    void setX(int);
    const int& getY();
    void setY(int);
    void setWidth(int);
    void setHeight(int);
    void draw(SDL_Renderer*);
    void resetYPos(int, int);
    ~Player();

private:
    int x;
    int y;
    int w;
    int h;
    AnimatedSprite* idleSprite;
    AnimatedSprite* runSprite;
    PlayerState state;
    Direction direction;
    int speed;
};

#endif // PLAYER_H_INCLUDED
