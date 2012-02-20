//////////////////////////////////////////////////////////////////////
// main.cpp
//////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

#include "common.h"
#include "data.h"
#include "jvm.h"

//////////////////////////////////////////////////////////////////////

using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace JB = Jarbinger;

//////////////////////////////////////////////////////////////////////

class Usage : public JB::Usage {
public:
    Usage(string binName) : _binName(binName) { /* nothing */ }

    virtual void output() {
        cout << "usage: " << _binName << " ..." << endl;
    }

private:
    string _binName;
};

//////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    int status= 0;
    int argNo= 0;
    string binName= argv[argNo++];
    Usage usage(binName);

    try {
        //TODO: command line options for debug and cleanup (e.g. -jb-clean)
        //   and pass remaining args onto tool
        string toolName(TOOL);
        string dataPath= JB::unArchive(toolName, &_binary_data_file_start,
                (JB::U64) &_binary_data_file_size);
        string mainJarFilename(dataPath +"/"+ JAR_FILENAME);
        string mainClassName(MAIN_CLASS_NAME);
        vector<string> vArgs;
        for (int i= argNo; i < argc; i++) {
            string arg(argv[i]);
            vArgs.push_back(arg);
        }
        JB::startJVM(mainJarFilename, mainClassName, vArgs);
    } catch (JB::Exception e) {
        e.reportAndExit();
    }

    return status;
}

//////////////////////////////////////////////////////////////////////

