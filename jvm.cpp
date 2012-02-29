//////////////////////////////////////////////////////////////////////
// jvm.cpp
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <vector>
#include <jni.h>

#include "jvm.h"
#include "common.h"

//////////////////////////////////////////////////////////////////////

namespace Jarbinger {

#include <string.h>

using std::string;
using std::vector;

//////////////////////////////////////////////////////////////////////

/**
 * create Java Virtual Machine (JVM) with given classpath and JVM opts
 */
static JNIEnv* createJVM(const string& classpath, const string& jvmOpts);

void startJVM(const string& mainJarFilename,
        const string& mainClassName, vector<string>& vArgs) {
    // construct classpath and create JVM
    string pkgSep(".");
    string pathSep("/");
    string pkgMainClassName(mainClassName);
    replace(pkgMainClassName, pathSep, pkgSep);
    string pathMainClassName(mainClassName);
    replace(pathMainClassName, pkgSep, pathSep);
    string jvmOpts("");
    //TODO: prepare JVM_OPTS
    JNIEnv* env= createJVM(mainJarFilename, jvmOpts);

    // find main class and main() method
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

    // prepare args and invoke main()
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

static JNIEnv* createJVM(const string& classpath, const string& jvmOpts) {
    JavaVM* jvm;
    JNIEnv* env;
    JavaVMInitArgs args;
    JavaVMOption options[1];

    // prepare JVM options
    args.version= JNI_VERSION_1_6;
    args.nOptions= 1;
    string classpathOpt("-Djava.class.path="+ classpath);
    options[0].optionString= strdup(classpathOpt.c_str());
    //TODO: fill jvmOpts into options
    args.options= options;
    args.ignoreUnrecognized= JNI_FALSE;

    // create JVM
    int createStatus= JNI_CreateJavaVM(&jvm, (void**)&env, &args);
    if (createStatus < 0) {
        //TODO: show options in message
        throw Exception("unable to create JVM");
    }
    return env;
}

//////////////////////////////////////////////////////////////////////

}

