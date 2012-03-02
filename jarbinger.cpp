//////////////////////////////////////////////////////////////////////
// jarbinger.cpp
//////////////////////////////////////////////////////////////////////

#include <string>
#include <boost/filesystem.hpp>

#include "common.h"
#include "data.h"

//////////////////////////////////////////////////////////////////////

using std::cout;
using std::endl;
using std::string;

namespace JB = Jarbinger;

//////////////////////////////////////////////////////////////////////

class Usage : public JB::Usage {
public:
    Usage(string binName) : _binName(binName) { /* nothing */ }

    virtual void output() {
        cout << "usage: " << _binName << " {toolName} {archiveDirPath} {jarFilename|.} {mainClassName}" << endl;
    }

private:
    string _binName;
};

//////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    int status= 0;
    int argNo= 0;
    string binName(argv[argNo++]);
    Usage usage(binName);
    try {
        string startDir= JB::getCwd();
        if (argNo >= argc) throw JB::Exception("missing tool name", &usage);
        boost::filesystem::path toolPath(argv[argNo++]);
        string toolName(toolPath.filename().c_str());
        string outputDirPath(toolPath.is_absolute()
                ? toolPath.parent_path().c_str() : startDir);
        if (argNo >= argc) throw JB::Exception("missing archive directory path", &usage);
        boost::filesystem::path tmpPath(argv[argNo++]);
        string archiveDirPath(tmpPath.is_absolute()
                ? tmpPath.c_str() : startDir +"/"+ tmpPath.c_str());
        if (! boost::filesystem::exists(archiveDirPath.c_str())) {
            throw JB::Exception("archive directory '"+ archiveDirPath +"' not found");
        }
        if (argNo >= argc) throw JB::Exception("missing jar filename", &usage);
        string jarFilename(argv[argNo++]);
        if (argNo >= argc) throw JB::Exception("missing main class name", &usage);
        string mainClassName(argv[argNo++]);
        string jvmOptsDefault("");
        // rest of arguments become default JVM options
        while (argNo < argc) {
            if (jvmOptsDefault != "") jvmOptsDefault+= " ";
            jvmOptsDefault+= argv[argNo];
            argNo++;
        }
        string jarbingerDataPath= JB::unArchive("jarbinger", &_binary_data_file_start,
                (JB::U64) &_binary_data_file_size);
        JB::setCwd(jarbingerDataPath);
        string makeCmd("make -s -C "+ jarbingerDataPath +" TOOL="+ toolName
                +" OUTPUT_DIR="+ outputDirPath
                +" ARCHIVE_DIR="+ archiveDirPath
                +" JAR_FILENAME="+ jarFilename
                +" MAIN_CLASS_NAME="+ mainClassName
                +" JVM_OPTS_DEFAULT="+ jvmOptsDefault);
        int systemStatus= system(makeCmd.c_str());
        if (systemStatus) {
            throw JB::Exception("executing '"+ makeCmd +"'");
        }
        string cleanupCmd("rm -rf "+ jarbingerDataPath);
        systemStatus= system(cleanupCmd.c_str());
        if (systemStatus) {
            throw JB::Exception("executing '"+ cleanupCmd +"'");
        }
    } catch (JB::Exception e) {
        e.reportAndExit();
    }
    return status;
}

//////////////////////////////////////////////////////////////////////

