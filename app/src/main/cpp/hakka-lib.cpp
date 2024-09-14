#include <jni.h>
#include <string>
#include "hakkaHelper.h"
#include "exception.h"

std::string packageName = "com.tencent.tmgp.sgame";
std::string nameGameCore = "libGameCore.so";
std::string nameIL2cpp = "libil2cpp.so";

std::shared_ptr<hakka::Target> process;

extern "C" JNIEXPORT jstring JNICALL
Java_fan_akua_hakka_Hakka_attachGame(JNIEnv *env,
                                     jclass) {
    pid_t pid;
    try {
        pid = hakka::findPidByPackage(packageName);
    } catch (hakka::no_process_error &e) {
        return env->NewStringUTF(e.what());
    }
    process = std::make_shared<hakka::Target>(pid, hakka::MemoryMode::MEM_FILE);
    return env->NewStringUTF("success");
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_fan_akua_hakka_Hakka_isAlve(JNIEnv *, jclass) {
    if (!process) {
        return JNI_FALSE;
    }
    return process->isAlive() ? JNI_TRUE : JNI_FALSE;
}