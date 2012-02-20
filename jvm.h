//////////////////////////////////////////////////////////////////////
// jvm.h
//////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

#ifndef JVM_H
#define JVM_H

namespace Jarbinger {

using std::string;
using std::vector;

void startJVM(const string& mainJarFilename,
        const string& mainClassName, vector<string>& vArgs);

}

#endif //JVM_H

