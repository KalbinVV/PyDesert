#include "Interpreter.h"
#include "Utility.h"
#include <Python.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

Interpreter::Interpreter() : prohibitedWords{"input","os", "iter"} {
    output << "";
}

void Interpreter::run(std::string source){
    std::string stdOutErr =
"import sys\n\
import threading\n\
oldStdOut = sys.stdout\n\
oldStdErr = sys.stderr\n\
fsock = open('python_out.log','w')\n\
sys.stdout = fsock\n\
sys.stderr = fsock"; //Пайтон код для редиректа вывода
    std::string mainCode = "\ncycle_flag = False\n\
if __name__ == '__main__':\n\
    p = threading.Thread(target = main)\n\
    #p.setDaemon(True)\n\
    p.start()\n\
    p.join(timeout=1)\n\
    cycle_flag = True\n\
    sys.stdout = oldStdOut\n\
    sys.stderr = oldStdErr\n\
    fsock.close()\n\
    sys.stdout.flush()";
    bool canRun = true;
    std::string outputStr;
    for(std::string word : prohibitedWords){
        if(source.find(word) != std::string::npos){
            outputStr = "К сожалению, данный функционал был отключен: " + word + "\nВозможно, в будущих версиях будет добавлена его поддержка!";
            canRun = false;
        }
    }
    Py_Initialize();
    PyRun_SimpleString(stdOutErr.c_str());
    std::vector<std::string> strings = Utility::splitString(source, '\n');
    if(strings[0] != "def main():"){
        canRun = false;
        outputStr = "В коде должна быть функция main!";
    }
    for(size_t i = 1; i < strings.size(); i++){
        if(strings[i][0] != ' ' && strings[i].length() != 0){
            canRun = false;
            outputStr = "Код вне главной функции не допускается!";
            break;
        }
    }
    size_t pos = 0;
    while((pos = source.find("while", pos)) != std::string::npos){
        source.insert(pos + 6, "not cycle_flag and ");
        pos = pos + 6;
    }

    mainCode.insert(0, source);
    auto start = std::chrono::high_resolution_clock::now();
    if(canRun){
        PyRun_SimpleString(mainCode.c_str());
    }
    Py_Finalize();
    auto now = std::chrono::high_resolution_clock::now();
    if(std::chrono::duration_cast<std::chrono::seconds>(now - start).count() >= 1){
        outputStr = "Превышено время ожидания!";
    }else{
        std::ifstream outputFile("python_out.log");
        std::string line;
        int k = 0;
        while(std::getline(outputFile, line)){
            outputStr += line + "\n";
            k++;
            if(k > 10){
                break;
            }
        }
        outputFile.close();
        output.clear();
        if(outputStr.find("cycle_flag") != std::string::npos){
            outputStr = "";
        }
        if(outputStr.find("Thread") != std::string::npos){
            std::vector<std::string> strings = Utility::splitString(outputStr, '\n');
            outputStr = strings[strings.size() - 4];
            outputStr += "\n" + strings[strings.size() - 3];
        }
    }
    output << outputStr;
}

void Interpreter::clean(){
    output.clear();
}

std::string Interpreter::getOutput(){
    return output.str();
}
