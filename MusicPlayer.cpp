#include <stdexcept>
#include <SDL.h>
#include "MusicPlayer.h"
#include "Utility.h"


MusicPlayer::MusicPlayer(std::vector<std::string> musics_){
    musics = musics_;
    itMusic = musics.begin();
    music = NULL;
}

void MusicPlayer::pause(){
    Mix_PauseMusic();
}

void MusicPlayer::play(){
    if(music != NULL){
        Mix_FreeMusic(music);
    }
    std::string musicPath = Utility::getPath("music",*itMusic);
    music = Mix_LoadMUS(musicPath.c_str());
    if(!music){
        SDL_Log("Can't load audio: %s", (*itMusic).c_str());
        next();
    }else{
        Mix_PlayMusic(music, 0);
    }
}

void MusicPlayer::update(){
    if(!Mix_PlayingMusic()){
        itMusic++;
        if(itMusic == musics.end()){
            itMusic = musics.begin();
        }
        play();
    }
}

void MusicPlayer::next(){
    itMusic++;
    if(itMusic == musics.cend()){
        itMusic = musics.begin();
    }
    play();
}

void MusicPlayer::prev(){
    if(itMusic == musics.begin()){
        itMusic = std::prev(musics.end());
    }else{
        itMusic--;
    }
    play();
}

std::string MusicPlayer::getCurrentMusic(){
    return *itMusic;
}

MusicPlayer::~MusicPlayer(){
    if(music){
        Mix_FreeMusic(music);
    }
}
