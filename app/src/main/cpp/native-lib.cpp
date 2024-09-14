#include <jni.h>
#include <string>
#include "hakkaHelper.h"
#include "exception.h"


std::string packageName = "com.tencent.tmgp.sgame";
std::string nameGameCore = "libGameCore.so";
std::string nameIL2cpp = "libil2cpp.so";

extern "C" JNIEXPORT jstring JNICALL
Java_fan_akua_hakka_demo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    pid_t pid;
    try {
        pid = hakka::findPidByPackage(packageName);
    } catch (hakka::no_process_error &e) {

    }
    auto process = std::make_shared<hakka::Target>(pid, hakka::MemoryMode::MEM_FILE);

    return env->NewStringUTF(hello.c_str());
}