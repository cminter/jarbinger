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

/**
 * contains info to output usage message
 */
class Usage {
public:
    /**
     * output usage message
     */
    virtual void output()= 0;
};

/**
 * exception with message, usage object, and exit code
 */
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

/**
 * copy internal data (expected to be an archive) to a file and un-archive it
 * @param toolName tool name used in file path
 * @param dataAddr data address
 * @param dataSize data size
 * @return path to data directory
 */
std::string unArchive(const std::string& toolName, char* dataAddr, const U64 dataSize);

/**
 * copy internal data to file
 * @param filename file name
 * @param dataAddr data address
 * @param dataSize data size
 */
void writeToFile(const std::string& filename, char* dataAddr, const U64 dataSize);

/**
 * get current working directory.
 * uses cwd() 
 * @return directory
 */
std::string getCwd();

/**
 * set current working directory.
 * uses chdir()
 * @param dirName directory
 * @return result of chdir()
 */
int setCwd(const std::string& dirName);

/**
 * replace all occurences of string with another
 * @param str string to be modified
 * @param oldText search text
 * @param newText replacement text
 */
void replace(std::string& str, const std::string& oldText,
        const std::string& newText);

}

//////////////////////////////////////////////////////////////////////
#endif //COMMON_H

