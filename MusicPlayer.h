#ifndef MUSICPLAYER_H_INCLUDED
#define MUSICPLAYER_H_INCLUDED

#include <vector>
#include <string>
#include <SDL_mixer.h>

class MusicPlayer{
public:
    MusicPlayer(std::vector<std::string>);
    void pause();
    void play();
    void update();
    void next();
    void prev();
    std::string getCurrentMusic();
    ~MusicPlayer();

private:
    Mix_Music* music;
    std::vector<std::string> musics;
    std::vector<std::string>::iterator itMusic;
};

#endif // MUSICPLAYER_H_INCLUDED
