#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include <sstream>
#include <vector>
#include <string>

class Interpreter{
public:
    Interpreter();
    void run(std::string);
    void clean();
    std::string getOutput();

private:
    std::stringstream output;
    const std::vector<std::string> prohibitedWords;
};

#endif // INTERPRETER_H_INCLUDED
