#include "Core.h"
#include <vector>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "Sprite.h"
#include "Level.h"
#include "Utility.h"
#include "Text.h"
#include "Interpreter.h"

Core::Core(const char* title, int w, int h, MusicPlayer* musicPlayer_, bool audioEnabled_){
    //Инициализация основных модулей
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0){
        SDL_Log("Can't init modules: %s", SDL_GetError());
        throw std::runtime_error("Can't init sdl modules!");
    }
    //Инициализация модуля шрифта и основных параметров
    window.createWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    IMG_Init(IMG_INIT_PNG);
    if(TTF_Init() != 0){
        SDL_Log("Can't init sdl ttf: %s", TTF_GetError());
        throw std::runtime_error("Can't init sdl ttf!");
    }
    renderer = SDL_CreateRenderer(window.getPtr(), -1, 0);
    if(renderer == NULL){
        SDL_Log("Can't init renderer: %s", SDL_GetError());
        throw std::runtime_error("Can't init renderer!");
    }
    state = GameState::Start;
    framePerSecond = 30;
    interactObject = nullptr;
    //Загрузка ресурсов
    //Персонаж
    player.setIdleSprite(Utility::getPath("assets","player(idle).png").c_str(), renderer, {
        SDL_Rect{x:0,y:0,w:32,h:32}, SDL_Rect{x:32,y:0,w:32,h:32}, SDL_Rect{x:64,y:0,w:32,h:32},
        SDL_Rect{x:96,y:0,w:32,h:32}, SDL_Rect{x:128,y:0,w:32,h:32}, SDL_Rect{x:160,y:0,w:32,h:32},
        SDL_Rect{x:192,y:0,w:32,h:32}, SDL_Rect{x:224,y:0,w:32,h:32}, SDL_Rect{x:256,y:0,w:32,h:32},
        SDL_Rect{x:288,y:0,w:32,h:32}, SDL_Rect{x:320,y:0,w:32,h:32}, SDL_Rect{x:352,y:0,w:32,h:32},
        SDL_Rect{x:384,y:0,w:32,h:32}
    });
    player.setRunSprite(Utility::getPath("assets","player(run).png").c_str(), renderer, {
        SDL_Rect{x:0,y:0,w:32,h:32}, SDL_Rect{x:32,y:0,w:32,h:32}, SDL_Rect{x:64,y:0,w:32,h:32},
        SDL_Rect{x:96,y:0,w:32,h:32}, SDL_Rect{x:128,y:0,w:32,h:32}, SDL_Rect{x:160,y:0,w:32,h:32},
        SDL_Rect{x:192,y:0,w:32,h:32}, SDL_Rect{x:224,y:0,w:32,h:32}
    });
    //Взаимодействие
    interactSpriteObject = new AnimatedSprite(Utility::getPath("assets","interact.png").c_str(), renderer, {
        SDL_Rect{x:0,y:0,w:16,h:16}, SDL_Rect{x:16,y:0,w:16,h:16}, SDL_Rect{x:32,y:0,w:16,h:16}
    });
    interactSpriteInfo = new AnimatedSprite(Utility::getPath("assets","interact.png").c_str(), renderer, {
        SDL_Rect{x:48,y:0,w:16,h:16}, SDL_Rect{x:64,y:0,w:16,h:16}, SDL_Rect{x:80,y:0,w:16,h:16}
    });
    //Инициализация музыки и звуков
    musicPlayer = musicPlayer_;
    audioEnabled = audioEnabled_;
    stepEffectChannel = 1;
    if(audioEnabled){
        stepEffect = Mix_LoadWAV(Utility::getPath("sound","steps.wav").c_str());
        if(stepEffect == NULL){
            SDL_Log("Can't load step effect: %s", Mix_GetError());
            throw std::runtime_error("Can't load step effect!");
        }
        Mix_PlayChannel(stepEffectChannel, stepEffect, -1);
        Mix_Pause(stepEffectChannel);
    }else{
        stepEffect = nullptr;
    }
    //Загрузка уровни и инициализация персонажа
    std::ifstream levelsFile(Utility::getPath("levels", "levels.txt"));
    std::string levelName;
    while(std::getline(levelsFile, levelName)){
        levels.push_back(Level(Utility::getPath("levels", levelName)));
    }
    if(levels.empty()){
        throw std::runtime_error("levels.txt is empty or not exist!");
    }
    levelsFile.close();
    currentLevel = 0;
    loadLevels();
    resetPlayer();
    //Загрузка иконки
    std::string iconPath = Utility::getPath("assets", "icon.png");
    SDL_Surface* icon = IMG_Load(iconPath.c_str());
    if(icon == NULL){
        SDL_Log("Can't load icon!\nPath:%s", iconPath.c_str());
        throw std::runtime_error("Can't load icon!");
    }
    SDL_SetWindowIcon(window.getPtr(), icon);
    SDL_FreeSurface(icon);
    SDL_StopTextInput();
    animState = true;
    musicButtonIcon = new Sprite(Utility::getPath("assets", "music.png").c_str(), renderer);
    playButtonIcon = new Sprite(Utility::getPath("assets", "play.png").c_str(), renderer);
    nextButtonIcon = new Sprite(Utility::getPath("assets", "next.png").c_str(), renderer);
    prevButtonIcon = new Sprite(Utility::getPath("assets", "prev.png").c_str(), renderer);
    exitButtonIcon = new Sprite(Utility::getPath("assets", "exit.png").c_str(), renderer);
    fullscreenButtonIcon = new Sprite(Utility::getPath("assets", "fullscreen.png").c_str(), renderer);
    compileButtonIcon = new Sprite(Utility::getPath("assets", "compile.png").c_str(), renderer);
    pauseButtonIcon = new Sprite(Utility::getPath("assets", "pause.png").c_str(), renderer);
    Sprite* settingsButtonIcon = compileButtonIcon;
    infoButtonIcon = new Sprite(Utility::getPath("assets", "info.png").c_str(), renderer);
    crossButtonIcon = new Sprite(Utility::getPath("assets", "cross.png").c_str(), renderer);
    helpButtonIcon = new Sprite(Utility::getPath("assets", "help.png").c_str(), renderer);
    buttons[GameState::Start] = {
        Button(0.5, 0.5, playButtonIcon, [&](){
            state = GameState::Main;
        }, "[Space] Запустить игру"),
        Button(0.5, 0.57, settingsButtonIcon, [&](){
            state = GameState::Settings;
        }, "[s] Настройки"),
        Button(0.5, 0.64, infoButtonIcon, [&](){
            state = GameState::Information;
        }, "[i] Информация"),
        Button(0.5, 0.71, exitButtonIcon, [&](){
            window.close();
        }, "[q] Выход")
    };
    buttons[GameState::Settings] = {
        Button(0.5, 0.5, musicButtonIcon, [&](){
            toggleSound();
        }, "[m] Музыка"),
        Button(0.5, 0.57, fullscreenButtonIcon, [&]{
            toggleFullscreen();
        }, "[F11] Полный экран"),
        Button(0.5, 0.64, exitButtonIcon, [&](){
            state = GameState::Start;
        }, "[q] Вернуться в меню")
    };
    buttons[GameState::Main] = {
        Button(0.80, 0, pauseButtonIcon, [&](){
            state = GameState::Pause;
        }),
        Button(0.84, 0, prevButtonIcon, [&]{
            if(audioEnabled){
                musicPlayer->prev();
            }
        }),
        Button(0.88, 0, nextButtonIcon, [&]{
            if(audioEnabled){
                musicPlayer->next();
            }
        }),
        Button(0.92, 0, musicButtonIcon, [&](){
            toggleSound();
        }),
        Button(0.96, 0, fullscreenButtonIcon, [&](){
            toggleFullscreen();
        })
    };
    buttons[GameState::Pause] = {
        Button(0.5, 0.5, playButtonIcon, [&](){
            state = GameState::Main;
        }, "[Space] Продолжить"),
        Button(0.5, 0.63, exitButtonIcon, [&](){
            state = GameState::Start;
        }, "[q] Вернуться в главное меню"),
        Button(0.5, 0.57, fullscreenButtonIcon, [&](){
            toggleFullscreen();
        }, "[F11] Полный экран")
    };
    buttons[GameState::Terminal] = {
        Button(0.96, 0, crossButtonIcon, [&]{
            state = GameState::Main;
        }),
        Button(0.92, 0, helpButtonIcon, [&]{
           state = GameState::TerminalHelp;
        }),
        Button(0, 0.95, compileButtonIcon, [&]{
            InterpreterInfo& interpreterInfo = levels[currentLevel].getInterpreterInfo();
            Interpreter interpreter;
            interpreter.run(interpreterInfo.sourceCode);
            interpreterInfo.output = interpreter.getOutput();
            terminalState = TerminalState::Output;
            if(Utility::splitString(interpreterInfo.output, '\n')[0] == levels[currentLevel].getRequiredOutput()){
                levels[currentLevel].setCompleted();
            }
        }, "[F5] Запустить"),
        Button(0.15, 0.95, infoButtonIcon, [&]{
            levels[currentLevel].getInterpreterInfo().output = levels[currentLevel].getDescription();
            terminalState = TerminalState::Description;
        }, "[ctrl+h] Информация")
    };
    buttons[GameState::TerminalHelp] ={
         Button(0.96, 0, crossButtonIcon, [&]{
            state = GameState::Terminal;
        })
    };
}

void Core::loadLevels(){
    for(Level& level : levels){
        level.load(renderer, window.getWidth(), window.getHeight());
    }
}

Window* Core::getWindow(){
    return &window;
}

void Core::loadFont(const char* path){
    font = TTF_OpenFont(path, 64);
    if(font == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Can't open font: %s", TTF_GetError());
    }
}

SDL_Renderer* Core::getRenderer(){
    return renderer;
}

void Core::setFps(int newFps){
    framePerSecond = newFps;
}

void Core::run(){
    render();
    Timer fpsTimer;
    while(window.isOpen()){
        fpsTimer.start();
        handleEvents();
        if(static_cast<int>(fpsTimer.getTicks()) < 1000 / framePerSecond ){
            SDL_Delay(1000 / framePerSecond - fpsTimer.getTicks());
        }
        if(musicPlayer != NULL && audioEnabled){
            musicPlayer->update();
        }
        render();
        update();
    }
}

void Core::toggleSound(){
    if(audioEnabled){
        musicPlayer->pause();
        audioEnabled = false;
    }else{
        musicPlayer->play();
        audioEnabled = true;
    }
}

void Core::toggleFullscreen(){
    if(!window.isFullscreen()){
        SDL_Log("Fullscreen enabled!");
        window.enableFullscreen();
    }else{
        SDL_Log("Fullscreen disabled!");
        window.disableFullscreen();
    }
    player.resetYPos(window.getHeight(), levels[currentLevel].getTileHeight());
}

void Core::handleEvents(){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:
                window.close();
                break;
            case SDL_WINDOWEVENT:
                switch(event.window.event){
                    case SDL_WINDOWEVENT_RESIZED:
                        /*
                        event.window.data1 - Новая ширина экрана
                        event.window.data2 - Новая высота экрана
                        */
                        SDL_Log("Window resized to %dx%d", event.window.data1, event.window.data2);
                        if(player.getX() != 0){
                            int delta =  player.getX() / (static_cast<float>(window.getWidth()) / 100);
                            player.setX(static_cast<float>(event.window.data1) / 100 * delta);
                        }
                        window.resizeWindow(event.window.data1, event.window.data2);
                        loadLevels();
                        resetPlayer();
                        render();
                        break;
                }
                break;
            case SDL_KEYDOWN:
                handleKeyDown(event);
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.scancode){
                    case SDL_SCANCODE_RIGHT:
                    case SDL_SCANCODE_LEFT:
                    case SDL_SCANCODE_D:
                    case SDL_SCANCODE_A:
                        if(state == GameState::Main){
                            player.setState(PlayerState::Idle);
                            player.setSpeed(Player::DEFAULT_SPEED);
                            if(audioEnabled) Mix_Pause(stepEffectChannel);
                        }
                        break;
                    default:
                        break;
                }
                break;
            //Ввод текста
            case SDL_TEXTINPUT:
                if(state == GameState::Terminal){
                    InterpreterInfo& interpreterInfo = levels[currentLevel].getInterpreterInfo();
                    if(static_cast<int>(*event.text.text) >= 0){
                        std::vector<std::string> lines = Utility::splitString(interpreterInfo.sourceCode, '\n');
                        lines[interpreterInfo.inputPosY].insert(interpreterInfo.inputPosX, event.text.text);
                        std::string tempSourceCode;
                        for(size_t i = 0; i < lines.size(); i++){
                            if(lines[i][lines[i].length() - 1] != '\n'){
                                if(i < lines.size() - 1){
                                    tempSourceCode += lines[i] + '\n';
                                }else{
                                    tempSourceCode += lines[i];
                                }
                            }
                        }
                        interpreterInfo.sourceCode = tempSourceCode;
                        interpreterInfo.inputPosX++;
                    }else{
                        interpreterInfo.output = "Используйте английскую раскладку, иная другая запрещена!";
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(event.button.button == SDL_BUTTON_LEFT){
                    SDL_Point point;
                    SDL_GetMouseState(&point.x, &point.y);
                    for(Button button : buttons[state]){
                        SDL_Rect rect = button.getRect(window.getWidth(), window.getHeight(), levels[currentLevel].getTileHeight() / 2);
                        if(SDL_PointInRect(&point, &rect)){
                            button.run();
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
}

void Core::handleKeyDown(SDL_Event& event){
    if(event.key.keysym.scancode != SDL_SCANCODE_LEFT && event.key.keysym.scancode != SDL_SCANCODE_D
    && event.key.keysym.scancode != SDL_SCANCODE_RIGHT && event.key.keysym.scancode != SDL_SCANCODE_A
    && player.getState() == PlayerState::Run){
        player.setState(PlayerState::Idle);
        player.setSpeed(Player::DEFAULT_SPEED);
        if(audioEnabled) Mix_Pause(stepEffectChannel);
    }
    switch(event.key.keysym.scancode){
        case SDL_SCANCODE_F11:
            if(!window.isFullscreen()){
                SDL_Log("Fullscreen enabled!");
                window.enableFullscreen();
            }else{
                SDL_Log("Fullscreen disabled!");
                window.disableFullscreen();
            }
            break;
        case SDL_SCANCODE_ESCAPE:
            switch(state){
                case GameState::Description:
                case GameState::Pause:
                    state = GameState::Main;
                    break;
                case GameState::Terminal:
                    state = GameState::Main;
                    SDL_StopTextInput();
                    break;
                case GameState::Main:
                    state = GameState::Pause;
                    break;
                case GameState::Information:
                case GameState::Settings:
                    state = GameState::Start;
                    break;
                case GameState::TerminalHelp:
                    state = GameState::Terminal;
                    break;
                default:
                    break;
                }
            break;
        case SDL_SCANCODE_Q:
            if(state == GameState::Start){
                window.close();
            }else if(state == GameState::Pause || state == GameState::Settings){
                state = GameState::Start;
            }
            break;
        case SDL_SCANCODE_S:
            if(state == GameState::Start){
                state = GameState::Settings;
            }
            break;
        case SDL_SCANCODE_I:
            if(state == GameState::Start){
                state = GameState::Information;
            }
            break;
        case SDL_SCANCODE_M:
            if(state == GameState::Settings || state == GameState::Main){
                if(audioEnabled){
                    musicPlayer->pause();
                    audioEnabled = false;
                }else{
                    musicPlayer->play();
                    audioEnabled = true;
                }
            }
            break;
        case SDL_SCANCODE_SPACE:
            if(state == GameState::Start || state == GameState::Pause){
                state = GameState::Main;
            }
            break;
        case SDL_SCANCODE_T:
            state = GameState::Terminal;
            SDL_StartTextInput();
            break;
        case SDL_SCANCODE_F5:
            if(state == GameState::Terminal){
                InterpreterInfo& interpreterInfo = levels[currentLevel].getInterpreterInfo();
                Interpreter interpreter;
                interpreter.run(interpreterInfo.sourceCode);
                interpreterInfo.output = interpreter.getOutput();
                terminalState = TerminalState::Output;
                if(Utility::splitString(interpreterInfo.output, '\n')[0] == levels[currentLevel].getRequiredOutput()){
                    levels[currentLevel].setCompleted();
                }
            }else{
                loadLevels();
                resetPlayer();
            }
            break;
        case SDL_SCANCODE_KP_PLUS:
            if(state == GameState::Main){
                if(musicPlayer) musicPlayer->next();
                if(!audioEnabled) audioEnabled = true;
            }
            break;
        case SDL_SCANCODE_KP_MINUS:
            if(state == GameState::Main){
                if(musicPlayer) musicPlayer->prev();
                if(!audioEnabled) audioEnabled = true;
            }
            break;
        case SDL_SCANCODE_RIGHT:
            if(state == GameState::Terminal){
                InterpreterInfo& interpreterInfo = levels[currentLevel].getInterpreterInfo();
                std::vector<std::string> lines = Utility::splitString(interpreterInfo.sourceCode, '\n');
                if(event.key.keysym.mod & KMOD_CTRL){
                    interpreterInfo.inputPosX = lines[interpreterInfo.inputPosY].length();
                }else{
                    if(interpreterInfo.inputPosX < static_cast<int>(lines[interpreterInfo.inputPosY].length())){
                        interpreterInfo.inputPosX++;
                    }else{
                        interpreterInfo.inputPosX = 0;
                    }
                }
                break;
            }
        case SDL_SCANCODE_D:
            if(state == GameState::Main){
                player.setState(PlayerState::Run);
                player.setDirection(Direction::Right);
                if(player.getSpeed() < Player::MAX_SPEED){
                    player.setSpeed(player.getSpeed() + 1);
                }
                player.setX(player.getX() + player.getSpeed());
                if(Mix_Paused(stepEffectChannel) && audioEnabled){
                    Mix_Resume(stepEffectChannel);
                }
            }
            break;
        case SDL_SCANCODE_LEFT:
            if(state == GameState::Terminal){
                InterpreterInfo& interpreterInfo = levels[currentLevel].getInterpreterInfo();
                std::vector<std::string> lines = Utility::splitString(interpreterInfo.sourceCode, '\n');
                if(event.key.keysym.mod & KMOD_CTRL){
                    interpreterInfo.inputPosX = 0;
                }else{
                    if(interpreterInfo.inputPosX > 0){
                        interpreterInfo.inputPosX--;
                    }else{
                        interpreterInfo.inputPosX = lines[interpreterInfo.inputPosY].length();
                    }
                }
                break;
            }
        case SDL_SCANCODE_A:
            if(state == GameState::Main){
                player.setState(PlayerState::Run);
                player.setDirection(Direction::Left);
                if(player.getSpeed() < Player::MAX_SPEED){
                    player.setSpeed(player.getSpeed() + 1);
                }
                player.setX(player.getX() - player.getSpeed());
                if(Mix_Paused(stepEffectChannel && audioEnabled)){
                    Mix_Resume(stepEffectChannel);
                }
            }
            break;
        case SDL_SCANCODE_E:
            if(interactObject){
                if(state == GameState::Main){
                    switch(interactObject->getType()){
                        case ObjectType::Description:
                        case ObjectType::DescriptionNoSee:
                            state = GameState::Description;
                            break;
                        case ObjectType::Terminal:
                            state = GameState::Terminal;
                            SDL_StartTextInput();
                            break;
                        }
                }
            }
            break;
            case SDL_SCANCODE_UP: //Стрелка вверх
                if(state == GameState::Terminal){
                    InterpreterInfo& interpreterInfo = levels[currentLevel].getInterpreterInfo();
                    std::vector<std::string> lines = Utility::splitString(interpreterInfo.sourceCode, '\n');
                    if(interpreterInfo.inputPosY > 0){
                        interpreterInfo.inputPosY--;
                        if(interpreterInfo.inputPosX > static_cast<int>(lines[interpreterInfo.inputPosY].length())){
                            interpreterInfo.inputPosX = lines[interpreterInfo.inputPosY].length();
                        }
                    }
                }
                break;
            case SDL_SCANCODE_DOWN: //Стрелка вниз
                if(state == GameState::Terminal){
                    InterpreterInfo& interpreterInfo = levels[currentLevel].getInterpreterInfo();
                    std::vector<std::string> lines = Utility::splitString(interpreterInfo.sourceCode, '\n');
                    if(interpreterInfo.inputPosY < static_cast<int>(lines.size() - 1)){
                        interpreterInfo.inputPosY++;
                        if(interpreterInfo.inputPosX > static_cast<int>(lines[interpreterInfo.inputPosY].length())){
                            interpreterInfo.inputPosX = lines[interpreterInfo.inputPosY].length();
                        }
                    }
                }
                break;
            case SDL_SCANCODE_H:
                if(state == GameState::Terminal && (event.key.keysym.mod & KMOD_CTRL)){
                    levels[currentLevel].getInterpreterInfo().output = levels[currentLevel].getDescription();
                    terminalState = TerminalState::Description;
                }
                break;
            case SDL_SCANCODE_BACKSPACE:
                if(state == GameState::Terminal){
                    InterpreterInfo& interpeterInfo = levels[currentLevel].getInterpreterInfo();
                    std::vector<std::string> lines = Utility::splitString(interpeterInfo.sourceCode, '\n');
                    if(event.key.keysym.mod & KMOD_CTRL){
                        auto it = lines.begin();
                        for(int i = 0; i < interpeterInfo.inputPosY; i++) it++;
                        lines.erase(it);
                        if(interpeterInfo.inputPosY != 0){
                            interpeterInfo.inputPosY--;
                            interpeterInfo.inputPosX = lines[interpeterInfo.inputPosY].length();
                        }else{
                            interpeterInfo.inputPosX = 0;
                        }
                    }else{
                        if(interpeterInfo.inputPosX != 0){
                            if(lines[interpeterInfo.inputPosY].length() > 0){
                                if(interpeterInfo.inputPosX > 0){
                                    lines[interpeterInfo.inputPosY].erase(interpeterInfo.inputPosX - 1, 1);
                                }else{
                                    lines[interpeterInfo.inputPosY].pop_back();
                                }
                                interpeterInfo.inputPosX--;
                            }
                        }else{
                            if(interpeterInfo.inputPosY > 0){
                                if(lines[interpeterInfo.inputPosY].empty()){
                                    auto it = lines.begin();
                                    for(int i = 0; i < interpeterInfo.inputPosY; i++) it++;
                                    lines.erase(it);
                                    interpeterInfo.inputPosY--;
                                    interpeterInfo.inputPosX = lines[interpeterInfo.inputPosY].length();
                                }else{
                                    std::string line = lines[interpeterInfo.inputPosY];
                                    auto itRemove = lines.begin();
                                    for(int i = 0; i < interpeterInfo.inputPosY; i++) itRemove++;
                                    lines.erase(itRemove);
                                    interpeterInfo.inputPosX = lines[interpeterInfo.inputPosY - 1].length();
                                    lines[interpeterInfo.inputPosY - 1] += line;
                                    interpeterInfo.inputPosY--;
                                }
                            }
                        }
                    }
                    std::string tempSourceCode;
                    for(size_t i = 0; i < lines.size(); i++){
                        if(lines[i][lines[i].length() - 1] != '\n'){
                            if(i < lines.size() - 1){
                                tempSourceCode += lines[i] + '\n';
                            }else{
                                tempSourceCode += lines[i];
                            }
                        }
                    }
                    interpeterInfo.sourceCode = tempSourceCode;
                }
                break;
            case SDL_SCANCODE_TAB:
                if(state == GameState::Terminal){
                    InterpreterInfo& interpreterInfo = levels[currentLevel].getInterpreterInfo();
                    std::vector<std::string> lines = Utility::splitString(interpreterInfo.sourceCode, '\n');
                    lines[interpreterInfo.inputPosY].insert(interpreterInfo.inputPosX, "   ");
                        std::string tempSourceCode;
                        for(size_t i = 0; i < lines.size(); i++){
                            if(lines[i][lines[i].length() - 1] != '\n'){
                                if(i < lines.size() - 1){
                                    tempSourceCode += lines[i] + '\n';
                                }else{
                                    tempSourceCode += lines[i];
                                }
                            }
                        }
                        interpreterInfo.sourceCode = tempSourceCode;
                        interpreterInfo.inputPosX += 3;
                }
                break;
            case SDL_SCANCODE_RETURN: //Enter
                if(state == GameState::Terminal){
                    const int maxLines = 20;
                    InterpreterInfo& interpreterInfo = levels[currentLevel].getInterpreterInfo();
                    std::vector<std::string> lines = Utility::splitString(interpreterInfo.sourceCode, '\n');

                    if(!lines[interpreterInfo.inputPosY].empty() && interpreterInfo.inputPosX == 0
                    && interpreterInfo.inputPosY < maxLines){
                        auto it = lines.begin();
                        for(int i = 0; i < interpreterInfo.inputPosY; i++) it++;
                        lines.insert(it, "");
                        interpreterInfo.inputPosY++;
                    }else{
                        if(!lines[interpreterInfo.inputPosY].empty() && interpreterInfo.inputPosX != 0
                    && interpreterInfo.inputPosY < maxLines &&
                    interpreterInfo.inputPosX != static_cast<int>(lines[interpreterInfo.inputPosY].length())){
                        std::string line = lines[interpreterInfo.inputPosY].substr(interpreterInfo.inputPosX,
                        lines[interpreterInfo.inputPosY].length() - interpreterInfo.inputPosX);
                        lines[interpreterInfo.inputPosY].erase(interpreterInfo.inputPosX,
                        lines[interpreterInfo.inputPosY].length() - interpreterInfo.inputPosX);
                        auto it = lines.begin();
                        for(int i = 0; i < interpreterInfo.inputPosY + 1; i++){
                            it++;
                        }
                        lines.insert(it, line);
                        interpreterInfo.inputPosY++;
                        interpreterInfo.inputPosX = 0;
                        }else{
                            if(interpreterInfo.inputPosY == static_cast<int>(lines.size() - 1) && interpreterInfo.inputPosY < maxLines){
                                lines.push_back("");
                                interpreterInfo.inputPosY++;
                                interpreterInfo.inputPosX = 0;
                            }else{
                                if(interpreterInfo.inputPosY < maxLines){
                                    auto it = lines.begin();
                                    for(int i = 0; i < interpreterInfo.inputPosY + 1; i++){
                                        it++;
                                    }
                                    lines.insert(it, "");
                                    interpreterInfo.inputPosY++;
                                    interpreterInfo.inputPosX = 0;
                                }
                            }
                        }
                    }
                    std::string tempSourceCode;
                    for(size_t i = 0; i < lines.size(); i++){
                        if(i < lines.size() - 1){
                            tempSourceCode += lines[i] + '\n';
                        }else{
                            tempSourceCode += lines[i];
                        }
                    }
                    interpreterInfo.sourceCode = tempSourceCode;
                }
                break;
            default:
                break;
    }
}

void Core::renderMusicInfo(){
    std::string musicStr;
    if(musicPlayer != NULL && audioEnabled){
        std::string musicName = musicPlayer->getCurrentMusic();
        musicStr = Utility::splitString(musicName, '.')[0];
    }else{
        musicStr = "Музыка приостановлена";
    }
    SDL_Rect dstRect{
        x:0,
        y:0,
        w: levels[currentLevel].getTileWidth() / 5 * static_cast<int>(musicStr.length()),
        h: levels[currentLevel].getTileHeight() / 2
    };
    Text text(musicStr, font);
    text.drawSolid(renderer, &dstRect, {0, 0, 0});
}

void Core::mainRender(){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    levels[currentLevel].draw(renderer);
    player.draw(renderer);
    renderMusicInfo();
    if(interactObject){
        SDL_Rect dstRect{
            x: player.getX(),
            y: player.getY() - 10,
            w: levels[currentLevel].getTileWidth() / 2,
            h: levels[currentLevel].getTileHeight() / 2
        };
        if(interactObject->getType() == ObjectType::Terminal){
            interactSpriteObject->drawAndUpdate(renderer, &dstRect);
        }else{
            interactSpriteInfo->drawAndUpdate(renderer, &dstRect);
        }
    }
}

void Core::render(){
    SDL_RenderClear(renderer);
    switch(state){
        case GameState::Start:{
            mainRender();
            std::string titleName = "PyDesert";
            Text text(titleName, font);
            SDL_Rect dstRect{
                x: window.getWidth() / 2,
                y: window.getHeight() / 2 - levels[currentLevel].getTileHeight() - levels[currentLevel].getTileHeight() / 10,
                w: levels[currentLevel].getTileWidth() / 4 * static_cast<int>(text.getString().length()),
                h: levels[currentLevel].getTileHeight()
            };
            text.drawSolid(renderer, &dstRect, {0, 0 ,0});
            break;
        }
        case GameState::Settings:{
            mainRender();
            std::string titleName = "Настройки";
            Text text(titleName, font);
            SDL_Rect dstRect{
                x: window.getWidth() / 2,
                y: window.getHeight() / 2 - levels[currentLevel].getTileHeight() - levels[currentLevel].getTileHeight() / 10,
                w: levels[currentLevel].getTileWidth() / 4 * static_cast<int>(text.getString().length()),
                h: levels[currentLevel].getTileHeight()
            };
            text.drawSolid(renderer, &dstRect, {0, 0 ,0});
            break;
        }
        case GameState::Information:{
            mainRender();
            std::string titleName = "Информация:";
            Text text(titleName, font);
            SDL_Rect dstRect{
                x: window.getWidth() / 2 - levels[currentLevel].getTileWidth() / 4 * static_cast<int>(text.getString().length()),
                y: window.getHeight() / 2 - levels[currentLevel].getTileHeight() - levels[currentLevel].getTileHeight() / 10,
                w: levels[currentLevel].getTileWidth() / 4 * static_cast<int>(text.getString().length()),
                h: levels[currentLevel].getTileHeight()
            };
            text.drawSolid(renderer, &dstRect, {0, 0 ,0});
            dstRect.x = window.getWidth() / 2;
            dstRect.h = levels[currentLevel].getTileHeight() / 3;
            dstRect.y += levels[currentLevel].getTileHeight();
            text.setString("Студент: В.В. Кальбин, 20КБ(с)РЗПО-2");
            dstRect.w = levels[currentLevel].getTileWidth() / 7 * static_cast<int>(text.getString().length());
            text.drawSolid(renderer, &dstRect, {0, 0, 0});
            dstRect.y += levels[currentLevel].getTileHeight() / 2;
            text.setString("Руководитель: К.Р. Джукашев");
            dstRect.w = levels[currentLevel].getTileWidth() / 7 * static_cast<int>(text.getString().length());
            text.drawSolid(renderer, &dstRect, {0, 0, 0});
            dstRect.y += levels[currentLevel].getTileHeight() / 2;
            text.setString("ESC - вернуться в главное меню");
            dstRect.w = levels[currentLevel].getTileWidth() / 7 * static_cast<int>(text.getString().length());
            text.drawSolid(renderer, &dstRect, {0, 0, 0});
            break;
        }
        case GameState::Main:{
            mainRender();
            break;
        }
        case GameState::Pause:{
            mainRender();
            Text text("Пауза", font);
            SDL_Rect dstRect{
                x: window.getWidth() / 2,
                y: window.getHeight() / 2 - levels[currentLevel].getTileHeight() - levels[currentLevel].getTileHeight() / 10,
                w: levels[currentLevel].getTileWidth() / 4 * static_cast<int>(text.getString().length()),
                h: levels[currentLevel].getTileHeight()
            };
            text.drawSolid(renderer, &dstRect, {0, 0, 0});
            break;
        }
        case GameState::Description:{
            mainRender();
            SDL_SetRenderDrawColor(renderer, 231, 231, 231, 110);
            SDL_Rect fillRect{
                x: 0,
                y: window.getHeight() - 100,
                w: window.getWidth(),
                h: 100
            };
            SDL_RenderFillRect(renderer, &fillRect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &fillRect);
            int y = window.getHeight() - 90;
            for(std::string str :  Utility::splitString(interactObject->getValue(), ';')){
                Text text(str, font);
                SDL_Rect dstRect{
                    x: 10,
                    y: y,
                    w: 6 * static_cast<int>(text.getString().length()),
                    h: 24
                };
                text.drawSolid(renderer, &dstRect, {0, 0, 0});
                y += 25;
            }
            Text text("ESC - Выйти из диалога", font);
            SDL_Rect dstRect{
                x: 10,
                y: y,
                w: 6 * static_cast<int>(text.getString().length()),
                h: 24
            };
            text.drawSolid(renderer, &dstRect, {0, 0, 0});
            break;
        }
        case GameState::Terminal:{
            SDL_SetRenderDrawColor(renderer, 231, 231, 231, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 35, 27, 27, 231);
            int y = 0;
            InterpreterInfo& interpreterInfo = levels[currentLevel].getInterpreterInfo();
            std::vector<std::string> lines = Utility::splitString(interpreterInfo.sourceCode, '\n');
            int delta = levels[currentLevel].getTileWidth() / 8;
            for(std::string line : lines){
                if(line.length() > 0){
                    Text text(std::to_string(y + 1), font);
                    SDL_Rect dstRect{
                        x: 0,
                        y: y * (delta * 3),
                        w: 12,
                        h: delta * 2
                    };
                    text.drawSolid(renderer, &dstRect, {0, 0, 0});
                    dstRect.x = 24;
                    text.setString(line);
                    dstRect.w = (delta + 2) * static_cast<int>(text.getString().length());
                    text.drawSolid(renderer, &dstRect, {0, 0, 0});
                }else{
                    Text text(std::to_string(y + 1), font);
                    SDL_Rect dstRect{
                        x: 0,
                        y: y * (delta * 3),
                        w: 12,
                        h: delta * 2
                    };
                    text.drawSolid(renderer, &dstRect, {0, 0, 0});
                }
                y++;
            }
            if(animState){
                SDL_Rect inputPosRect{
                    x: (interpreterInfo.inputPosX) * (delta + 2) + 24,
                    y: interpreterInfo.inputPosY * (delta * 3),
                    w: 1,
                    h: (delta * 2)
                };
                SDL_RenderDrawRect(renderer, &inputPosRect);
                animState = false;
            }else{
                animState = true;
            }

            SDL_Rect dstRect;
            dstRect.h = delta * 3;
            SDL_RenderDrawLine(renderer, 0, window.getHeight() - 220, window.getWidth(), window.getHeight() - 220);
            if(!interpreterInfo.output.empty()){
                int k = 0;
                y = window.getHeight() - 200;
                dstRect.x = 0;
                for(std::string line : Utility::splitString(interpreterInfo.output,
                (terminalState == TerminalState::Output ? '\n' : ';'))){
                    if(!line.empty() && line != "\n"){
                        if(terminalState == TerminalState::Description){
                            if(line[0] == ' ') line.erase(0, 1);
                        }
                        bool isRu = false;
                        for(int j = 0; j < static_cast<int>(line.length()); j++){
                            if(static_cast<int>(line[j]) < 0){
                                isRu = true;
                                break;
                            }
                        }
                        Text text(line, font);
                        if(isRu){
                            dstRect.w = delta * static_cast<int>(text.getString().length());
                        }else{
                            dstRect.w = (delta + 2) * static_cast<int>(text.getString().length());
                        }
                        dstRect.y = y;
                        text.drawSolid(renderer, &dstRect, {0, 0, 0});
                        y += delta * 3;
                        k++;
                    }
                    if(k == 5) break;
                }
            }
            if(levels[currentLevel].isCompleted()){
                dstRect.y += delta * 3;
                dstRect.x = 0;
                dstRect.h = delta * 2;
                Text text("Уровень пройден!", font);
                dstRect.w = (delta + 2) * static_cast<int>(text.getString().length());
                text.drawSolid(renderer, &dstRect, {0, 100, 0});
            }
            break;
        }
        case GameState::TerminalHelp:{
            SDL_RenderClear(renderer);
            Text text("Стрелочки - перемещение по текстовому редактору", font);
            SDL_Rect dstRect{
                x: 0,
                y: 0,
                w: 6 * static_cast<int>(text.getString().length()),
                h: 24
            };
            text.drawSolid(renderer, &dstRect, {0, 0, 0});
            text.setString("CTRL + Стрелочка влево - перепрыгнуть на левый край строки");
            dstRect.y += 26;
            dstRect.w = 6 * static_cast<int>(text.getString().length());
            text.drawSolid(renderer, &dstRect, {0, 0, 0});
            text.setString("CTRL + Стрелочка вправо - перепрыгнуть на правый край строки");
            dstRect.y += 26;
            dstRect.w = 6 * static_cast<int>(text.getString().length());
            text.drawSolid(renderer, &dstRect, {0, 0, 0});
            text.setString("ENTER - переход на следующею строку");
            dstRect.y += 52;
            dstRect.w = 6 * static_cast<int>(text.getString().length());
            text.drawSolid(renderer, &dstRect, {0, 0, 0});
            text.setString("TAB - табуляция (3 пробела)");
            dstRect.y += 26;
            dstRect.w = 6 * static_cast<int>(text.getString().length());
            text.setString("CTRL + BACKSPACE - удалить всю строчку");
            dstRect.y += 26;
            dstRect.w = 6 * static_cast<int>(text.getString().length());
            text.drawSolid(renderer, &dstRect, {0, 0, 0});
            text.drawSolid(renderer, &dstRect, {0, 0, 0});
            text.setString("ESC - вернуться в редактор кода");
            dstRect.y += 52;
            dstRect.w = 6 * static_cast<int>(text.getString().length());
            text.drawSolid(renderer, &dstRect, {0, 0, 0});
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            break;
        }
    }
    for(Button button : buttons[state]){
        button.draw(renderer, font, window.getWidth(), window.getHeight(), levels[currentLevel].getTileHeight() / 2);
    }
    SDL_RenderPresent(renderer);
}

void Core::resetPlayer(){
    player.setWidth(levels[currentLevel].getTileWidth());
    player.setHeight(levels[currentLevel].getTileHeight());
    player.resetYPos(window.getHeight(), levels[currentLevel].getTileHeight());
}

void Core::update(){
    if(player.getX() > window.getWidth()){
        if(currentLevel < static_cast<int>(levels.size()) - 1 && levels[currentLevel].isCompleted()){
            currentLevel++;
            resetPlayer();
            player.setX(0);
        }else{
            player.setX(window.getWidth() - levels[currentLevel].getTileWidth());
        }
    }else if(player.getX() < -levels[currentLevel].getTileWidth()){
        if(currentLevel != 0){
            currentLevel--;
            resetPlayer();
            player.setX(window.getWidth() - 10);
        }else{
            player.setX(0);
        }
    }
    checkCollision();
}

void Core::checkCollision(){
    SDL_Rect playerRect{
        x: player.getX(),
        y: player.getY(),
        w: levels[currentLevel].getTileWidth(),
        h: levels[currentLevel].getTileHeight()
    };
    interactObject = nullptr;
    std::vector<LevelObject>& objects = levels[currentLevel].getObjects();
    for(size_t i = 0; i < objects.size(); i++){
        SDL_Rect objRect{
            x: objects[i].getX(),
            y: player.getY(),
            w: levels[currentLevel].getTileWidth(),
            h: levels[currentLevel].getTileHeight()
        };
        if(Utility::intersect(playerRect, objRect)){
            interactObject = &objects[i];
        }
    }
}

Core::~Core(){
    delete interactSpriteObject;
    delete interactSpriteInfo;
    delete musicButtonIcon;
    delete playButtonIcon;
    delete exitButtonIcon;
    delete fullscreenButtonIcon;
    delete compileButtonIcon;
    delete infoButtonIcon;
    delete pauseButtonIcon;
    delete crossButtonIcon;
    delete nextButtonIcon;
    delete prevButtonIcon;
    delete helpButtonIcon;
    SDL_DestroyRenderer(renderer);
    if(stepEffect && audioEnabled) Mix_FreeChunk(stepEffect);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}
