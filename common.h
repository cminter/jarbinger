//////////////////////////////////////////////////////////////////////
// common.h
//////////////////////////////////////////////////////////////////////

#ifndef COMMON_H
#define COMMON_H
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <stdlib.h>

namespace Jarbinger {

typedef u_int64_t U64;

class Usage {
public:
    virtual void output()= 0;
};

class Exception {
public:
    Exception(std::string message, Usage* usage= NULL, int exitCode= 1)
        : _message(message), _usage(usage), _exitCode(exitCode) { }

    std::string getMessage() { return _message; }

    int getExitCode() { return _exitCode; }

    void report() {
        std::cout << "ERROR: " << _message << std::endl;
    }

    void reportAndExit() {
        report();
        if (_usage) _usage->output();
        exit(_exitCode);
    }

private:
    std::string _message;
    Usage* _usage;
    int _exitCode;
};

std::string unArchive(const std::string& toolName, char* dataAddr, const U64 dataSize);
void writeToFile(const std::string& filename, char* dataAddr, const U64 dataSize);
std::string getCwd();
int setCwd(const std::string& dirName);
void replace(std::string& str, const std::string& oldText,
        const std::string& newText);

}

//////////////////////////////////////////////////////////////////////
#endif //COMMON_H

