#include "Utility.h"

std::vector<int> Utility::splitStringInt(const std::string& str, const char& separator){
    size_t pos = 0;
    size_t start_pos = 0;
    std::vector<int> result;
    while((pos = str.find(separator, start_pos)) != std::string::npos){
        result.push_back(std::stoi(str.substr(start_pos, pos - start_pos)));
        start_pos = pos + 1;
    }
    result.push_back(std::stoi(str.substr(start_pos, str.length() - start_pos)));
    return result;
}

std::vector<std::string> Utility::splitString(const std::string& str, const char& separator){
    size_t pos = 0;
    size_t start_pos = 0;
    std::vector<std::string> result;
    while((pos = str.find(separator, start_pos)) != std::string::npos){
        result.push_back(str.substr(start_pos, pos - start_pos));
        start_pos = pos + 1;
    }
    result.push_back(str.substr(start_pos, str.length() - start_pos));
    return result;
}

char Utility::getOsSeparator(){
    #if defined _WIN32 || defined __CYGWIN__
        return '\\';
    #else
        return '/';
    #endif
}

std::string Utility::getPath(std::string folder, std::string path){
    #if defined _WIN32 || defined __CYGWIN__
        std::string finalPath = ".\\";
        finalPath += folder;
        finalPath += getOsSeparator();
        finalPath += path;
        return finalPath;
    #else
        std::string finalPath = folder;
        finalPath += getOsSeparator();
        finalPath += path;
        return finalPath;
    #endif
}

bool Utility::intersect(SDL_Rect firstRect, SDL_Rect secondRect){
    return SDL_HasIntersection(&firstRect,&secondRect) == SDL_TRUE ? true : false;
}

