//////////////////////////////////////////////////////////////////////
// common.cpp
//////////////////////////////////////////////////////////////////////

#include <sstream>
#include <fstream>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <sys/stat.h>

#include "common.h"
#include "data.h"

//////////////////////////////////////////////////////////////////////

namespace Jarbinger {

using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::ofstream;

//////////////////////////////////////////////////////////////////////

#define MODE_DEFAULT (S_IRUSR | S_IWUSR  | S_IXUSR)

//////////////////////////////////////////////////////////////////////

/**
 * manages a working directory (temporary by default) that
 * is deleted when WorkDir goes out of scope
 */
class WorkDir {
public:
    WorkDir(string path, mode_t mode= MODE_DEFAULT) : _path(path), _mode(mode) {
        boost::filesystem::path bfPath(path);
        mkdirPath(bfPath);
        setPermission(mode);
        _keep= false;
        _lastPath= getCwd();
        _stay= false;
        if (setCwd(_path)) {
            throw Exception("unable to chdir to '"+ _path +"'");
        }
    }

    void keep() { _keep= true; }

    void stay() { _stay= true; }

    void setPermission(mode_t mode= MODE_DEFAULT) {
        if (chmod(_path.c_str(), mode)) {
            throw Exception("unable to chmod '"+ _path +"'");
        }
    }

    ~WorkDir() {
        if (! _stay) {
            if (setCwd(_lastPath)) {
                throw Exception("unable to chdir to '"+ _lastPath +"'");
            }
        }
        if (! _keep) {
            string cleanupCmd("rm -rf "+ _path);
            int systemStatus= system(cleanupCmd.c_str());
            if (systemStatus) {
                throw Exception("executing '"+ cleanupCmd +"'");
            }
        }
    }

private:
    void mkdirPath(boost::filesystem::path path) {
        boost::filesystem::path parent= path.parent_path();
        if (! boost::filesystem::exists(parent.c_str())) {
            mkdirPath(parent);
        }
        if (mkdir(path.c_str(), _mode)) {
            string p(path.c_str());
            throw Exception("unable to mkdir '"+ p +"'");
        }
        
    }

    string _path;
    mode_t _mode;
    bool _keep;
    string _lastPath;
    bool _stay;
};

class WorkFile {
public:
    WorkFile(string path) : _path(path) { /* nothing */ }

    void keep() { _keep= true; }

    ~WorkFile() {
        if (! _keep) {
            if (unlink(_path.c_str())) {
                throw Exception("removing '"+ _path +"'");
            }
        }
    }

private:
    string _path;
    bool _keep;
};

//////////////////////////////////////////////////////////////////////

string unArchive(const string& toolName, char* dataAddr, const U64 dataSize) {
    string archiveTail("data.tar.bz2");
    string logname(getenv("LOGNAME"));
    stringstream cacheRootS;
    cacheRootS << boost::format(JARBINGER_CACHE_PATH_FORMAT) % logname % toolName % BUILD_TAG;
    string cacheDirPath(cacheRootS.str());
    string archiveFilename(cacheDirPath +"/"+ archiveTail);
    if (! boost::filesystem::exists(cacheDirPath.c_str())) {
        // only done if does not already exist
        WorkDir cacheDir(cacheDirPath);
        WorkFile archiveFile(archiveFilename);
        writeToFile(archiveFilename, dataAddr, dataSize );
        stringstream unarchiveCmdS;
        unarchiveCmdS << boost::format(UNARCHIVE_CMD_FORMAT)
                % archiveFilename.c_str();
        int systemStatus= system(unarchiveCmdS.str().c_str());
        if (systemStatus) {
            throw Exception("executing '"+ unarchiveCmdS.str() +"'");
        }
        cacheDir.keep();
    }
    return cacheDirPath;
}

void writeToFile(const string& filename, char* dataAddr, const U64 dataSize) {
    ofstream os(filename.c_str(), std::ios::binary);
    os.write(dataAddr, dataSize);
    os.close();
}

string getCwd() {
    char* cwdTmp= getcwd(NULL, 0);
    string cwd(cwdTmp);
    free(cwdTmp);
    return cwd;
}

int setCwd(const string& dirName) {
    return chdir(dirName.c_str());
}

void replace(string& str, const string& oldText, const string& newText) {
    size_t pos= 0;
    while((pos= str.find(oldText, pos)) != string::npos) {
        str.replace(pos, oldText.length(), newText);
        pos+= newText.length();
    }
}

//////////////////////////////////////////////////////////////////////

}

