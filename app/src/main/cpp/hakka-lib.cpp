#include <jni.h>
#include <string>
#include <android/log.h>
#include <string>
#include <istream>
#include <sstream>
#include <cctype>
#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <sstream>
#include "hakkaHelper.h"
#include "exception.h"

std::string packageName = "com.tencent.tmgp.sgame";
std::string nameGameCore = "libGameCore.so";

std::shared_ptr<hakka::Target> process;

#define LOG_TAG "simonServer"
// 10408 332
// 代码7dbef40548
// 生命7dbef404b4

// 代码7937623770
// 生命79376237f8

// https://www.bilibili.com/video/BV1F841177T1/?spm_id_from=333.337.search-card.all.click
extern "C" JNIEXPORT jstring JNICALL
Java_fan_akua_hakka_Hakka_attachGame(JNIEnv *env,
                                     jclass) {
    pid_t pid;
    try {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "start findPid");
        pid = hakka::findPidByPackage(packageName);
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "find!!: %d", pid);
    } catch (hakka::no_process_error &e) {
        return env->NewStringUTF(e.what());
    }
    process = std::make_shared<hakka::Target>(pid, hakka::MemoryMode::MEM_FILE);

    auto maps = process->getAllMaps();
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "start scan!!");
    std::shared_ptr<hakka::ProcMaps> gameCore;
    for (auto &item: maps) {
        if (strstr(item->module_name, nameGameCore.c_str())) {
            gameCore = item;
            break;
        }
    }
    if (gameCore != nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "find core me!!: %d", gameCore->start());
    }
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

extern "C"
JNIEXPORT void JNICALL
Java_fan_akua_hakka_Hakka_search(JNIEnv *env, jclass clazz) {
    auto searcher = hakka::MemorySearcher(process);
//    searcher.setMemoryRange(hakka::MemoryRange::OTHER | hakka::MemoryRange::CA);
    searcher.setMemoryRange(hakka::MemoryRange::OTHER);
//    searcher.setMemoryRange(hakka::MemoryRange::ALL);
    searcher.setPageConfig(false, false);
    searcher.setSearchRange(0, 0xfffffffff);
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "pre search");
    auto size = searcher.searchValue("2680D;170D", 512);
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "find num: %d", size);

    auto mySet = searcher.getResults();
    i32 tmp;
    for (const auto &ptr: mySet) {
        process->read(ptr, &tmp, sizeof(tmp));
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "find %d [%llx]", tmp, ptr);
    }
}