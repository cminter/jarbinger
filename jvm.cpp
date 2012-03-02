//////////////////////////////////////////////////////////////////////
// jvm.cpp
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <jni.h>

#include "jvm.h"
#include "common.h"
#include "data.h"

//////////////////////////////////////////////////////////////////////

namespace Jarbinger {

#include <string.h>

using std::string;
using std::vector;

//////////////////////////////////////////////////////////////////////

static JNIEnv* createJVM(const string& classpath);

void startJVM(const string& mainJarFilename,
        const string& mainClassName, vector<string>& vArgs) {
    //TODO: use JVM_OPTS
    string pkgSep(".");
    string pathSep("/");
    string pkgMainClassName(mainClassName);
    replace(pkgMainClassName, pathSep, pkgSep);
    string pathMainClassName(mainClassName);
    replace(pathMainClassName, pkgSep, pathSep);
    JNIEnv* env= createJVM(mainJarFilename);
    jclass mainClass= env->FindClass(pathMainClassName.c_str());
    if (0 == mainClass) {
        if (env->ExceptionOccurred()) env->ExceptionDescribe();
        throw Exception("unable to find class '"+ pkgMainClassName +"'");
    }
    jmethodID mainMethodID= env->GetStaticMethodID(mainClass, "main",
            "([Ljava/lang/String;)V");
    if (0 == mainMethodID) {
        if (env->ExceptionOccurred()) env->ExceptionDescribe();
        throw Exception("unable to find main method in '"+ pkgMainClassName +"'");
    }
    jobjectArray args= env->NewObjectArray(vArgs.size(),
            env->FindClass("java/lang/String"), 0);  
    int argNo= 0;
    vector<string>::iterator vArgsIt;
    for (vArgsIt= vArgs.begin(); vArgsIt != vArgs.end(); vArgsIt++) {
        jstring arg= env->NewStringUTF((*vArgsIt).c_str());  
        env->SetObjectArrayElement(args, argNo, arg);  
        argNo++;
    }
    env->CallStaticVoidMethod(mainClass, mainMethodID, args);
}

//////////////////////////////////////////////////////////////////////

static JNIEnv* createJVM(const string& classpath) {
    JavaVM* jvm;
    JNIEnv* env;
    JavaVMInitArgs args;

    args.version= JNI_VERSION_1_6;

    vector<string> jvmOptions;
    // add default or user environment options
    string tool(TOOL);
    string jvmOptionsEnvName(tool +"_JVM_OPTS");
    char* userJVMOptions= getenv(jvmOptionsEnvName.c_str());
    string jvmOptionsExtraS(userJVMOptions ? userJVMOptions
            : JVM_OPTS_DEFAULT);
    if (jvmOptionsExtraS != "") {
        boost::split(jvmOptions, jvmOptionsExtraS, boost::is_any_of(" "));
    }
    // add classpath
    string classpathOpt("-Djava.class.path="+ classpath);
    jvmOptions.push_back(classpathOpt);
    // convert vector to 'options' array
    JavaVMOption* options= new JavaVMOption[jvmOptions.size()];
    vector<string>::iterator joIt;
    int optNo= 0;
    for (joIt= jvmOptions.begin(); joIt != jvmOptions.end(); joIt++) {
        options[optNo].optionString= strdup((*joIt).c_str());
        optNo++;
    }
    args.options= options;
    args.nOptions= optNo;
    args.ignoreUnrecognized= JNI_FALSE;

    int createStatus= JNI_CreateJavaVM(&jvm, (void**)&env, &args);
    if (createStatus < 0) {
        //TODO: error handling
    }
    return env;
}

//////////////////////////////////////////////////////////////////////

}

