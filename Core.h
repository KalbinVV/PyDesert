#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

#include <vector>
#include <SDL_ttf.h>
#include <map>
#include "Window.h"
#include "AnimatedSprite.h"
#include "Timer.h"
#include "Player.h"
#include "MusicPlayer.h"
#include "Level.h"
#include "Button.h"

class Core{
public:
    Core(const char*, int, int, MusicPlayer* = NULL, bool = true);
    Window* getWindow();
    void loadFont(const char*);
    SDL_Renderer* getRenderer();
    void setFps(int);
    void run();
    ~Core();

private:
    enum class GameState{
        Start, Settings, Information,
        Main, Pause, Description,
        Terminal, TerminalHelp
    };
    enum class TerminalState{
        Output,
        Description
    };

    GameState state;
    Window window;
    SDL_Renderer* renderer;
    int framePerSecond; //Максимальный ФПС
    TTF_Font* font;
    Player player;
    std::map<GameState, std::vector<Button>> buttons;

    std::vector<Level> levels;
    int currentLevel;

    MusicPlayer* musicPlayer;
    bool audioEnabled;
    Mix_Chunk* stepEffect;
    int stepEffectChannel;

    AnimatedSprite* interactSpriteObject;
    AnimatedSprite* interactSpriteInfo;
    Sprite* musicButtonIcon;
    Sprite* playButtonIcon;
    Sprite* exitButtonIcon;
    Sprite* fullscreenButtonIcon;
    Sprite* compileButtonIcon;
    Sprite* infoButtonIcon;
    Sprite* pauseButtonIcon;
    Sprite* crossButtonIcon;
    Sprite* nextButtonIcon;
    Sprite* prevButtonIcon;
    Sprite* helpButtonIcon;

    LevelObject* interactObject;

    bool animState;
    TerminalState terminalState;

    void toggleSound();
    void toggleFullscreen();
    void handleEvents();
    void handleKeyDown(SDL_Event&);
    void render();
    void renderMusicInfo();
    void mainRender();
    void loadLevels();
    void update();
    void checkCollision();
    void resetPlayer(); //Функция для подгона персонажа при смене уровня/резайза.
};

#endif // CORE_H_INCLUDED
