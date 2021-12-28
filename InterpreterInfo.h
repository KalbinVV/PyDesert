#ifndef INTERPRETERINFO_H_INCLUDED
#define INTERPRETERINFO_H_INCLUDED

#include <string>

class InterpreterInfo{
public:
    std::string sourceCode;
    std::string output;
    int inputPosX;
    int inputPosY;
    InterpreterInfo();
};

#endif // INTERPRETERINFO_H_INCLUDED
