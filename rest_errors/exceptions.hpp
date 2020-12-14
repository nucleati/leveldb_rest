#ifndef EXPECTIONS_THROUGHOUT_THE_EXECUTION
#define EXPECTIONS_THROUGHOUT_THE_EXECUTION
#include <iostream>
#include <string>
#include <map>
#include <sstream>
class LevelDBRestException: public std::exception {
    public:
    LevelDBRestException(const std::string& message):message(message){};
    LevelDBRestException(){
        message = "Problem occured while executing your request.";
    };
    virtual const char* what() {
       return message.c_str();
    }
    std::string message ;
};
#endif