#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include <vector>
#include <string>
#include <SDL.h>

namespace Utility{
    std::vector<int> splitStringInt(const std::string& str, const char& separator);
    std::vector<std::string> splitString(const std::string& str, const char& separator);
    char getOsSeparator();
    std::string getPath(std::string, std::string);
    bool intersect(SDL_Rect, SDL_Rect);
};

#endif // UTILITY_H_INCLUDED
