#include <iostream>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <SDL.h>
#include <SDL_mixer.h>
#include <Python.h>
#include "Core.h"
#include "Utility.h"

int main(int argc, char* args[]){
    bool audioEnabled = true;
    try{
        if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ){
            SDL_Log("Can't init sdl_mixer: %s", Mix_GetError());
            throw std::runtime_error("Can't init sdl mixer!");
        }
    }catch(std::runtime_error& e){
        std::cout << "Run time error: " << e.what() << std::endl;
        audioEnabled = false;
    }
    Mix_Init(MIX_INIT_OGG);
    std::vector<std::string> musics;
    {
        std::ifstream songsFile(Utility::getPath("music", "songs.txt"));
        std::string musicName;
        while(std::getline(songsFile, musicName)){
            musics.push_back(musicName);
        }
        songsFile.close();
    }
    if(musics.empty()){
        SDL_Log("Can't init audio: songs.txt is empty or not exist!");
        audioEnabled = false;
    }
    MusicPlayer musicPlayer(musics);

    Core* core = nullptr;
    bool successInit = true;
    try{
        core = new Core("PyDesert", 800, 600, (audioEnabled ? &musicPlayer : NULL), audioEnabled);
    }catch(std::runtime_error& e){
        std::cout << "Run time error when init core: " << e.what() << std::endl;
        successInit = false;
    }catch(...){
        std::cout << "Undefined error when init core!" << std::endl;
        successInit = false;
    }
    if(successInit){
        try{
            core->loadFont("font.ttf");
            core->run();
        }catch(std::runtime_error& e){
            std::cout << "Run time error: " << e.what() << std::endl;
        }catch(...){
            std::cout << "Undefined error!" << std::endl;
        }
    }
    Mix_Quit();
    if(core){
        delete core;
    }
    return 0;
}
