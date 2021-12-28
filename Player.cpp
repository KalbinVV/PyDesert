#include "Player.h"

const int Player::DEFAULT_SPEED = 2;
const int Player::MAX_SPEED = 7;

Player::Player(){
    x = 0;
    y = 0;
    h = 32;
    w = 32;
    state = PlayerState::Idle;
    direction = Direction::Right;
    speed = Player::DEFAULT_SPEED;
}

void Player::setIdleSprite(const char* path, SDL_Renderer* renderer, std::vector<SDL_Rect> rects){
    idleSprite = new AnimatedSprite(path, renderer, rects);
}

void Player::setRunSprite(const char* path, SDL_Renderer* renderer, std::vector<SDL_Rect> rects){
    runSprite = new AnimatedSprite(path, renderer, rects);
}

PlayerState Player::getState(){
    return state;
}

void Player::setState(PlayerState newState){
    state = newState;
}

void Player::setDirection(Direction newDirection){
    direction = newDirection;
}

void Player::setSpeed(int newSpeed){
    speed = newSpeed;
}

const int& Player::getSpeed(){
    return speed;
}

const int& Player::getX(){
    return x;
}

void Player::setX(int newX){
    x = newX;
}

const int& Player::getY(){
    return y;
}

void Player::setY(int newY){
    y = newY;
}

void Player::setWidth(int newWidth){
    w = newWidth;
}

void Player::setHeight(int newHeight){
    h = newHeight;
}


void Player::draw(SDL_Renderer* renderer){
    switch(state){
        case PlayerState::Idle:{
            switch(direction){
                case Direction::Right:{
                    SDL_Rect dstRect{
                        x: x,
                        y: y,
                        w: w,
                        h: h
                    };
                    idleSprite->drawAndUpdate(renderer, &dstRect);
                    break;
                }
                case Direction::Left:{
                    SDL_Rect dstRect{
                        x: x,
                        y: y,
                        w: w,
                        h: h
                    };
                    idleSprite->drawAndUpdateR(renderer, &dstRect, 0, SDL_FLIP_HORIZONTAL);
                    break;
                }
            }
        }
        break;
        case PlayerState::Run:{
            switch(direction){
                case Direction::Right:{
                    SDL_Rect dstRect{
                        x: x,
                        y: y,
                        w: w,
                        h: h
                    };
                    runSprite->drawAndUpdate(renderer, &dstRect);
                    break;
                }
                break;
                case Direction::Left:{
                    SDL_Rect dstRect{
                        x: x,
                        y: y,
                        w: w,
                        h: h
                    };
                    runSprite->drawAndUpdateR(renderer, &dstRect, 0, SDL_FLIP_HORIZONTAL);
                    break;
                }
                break;
            }
        }
        break;
    }
}

void Player::resetYPos(int windowHeight, int tileHeight){
    y = windowHeight - tileHeight * 2;
}

Player::~Player(){
    delete idleSprite;
    delete runSprite;
}
