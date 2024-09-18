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

std::shared_ptr<hakka::Target> process;

#define LOG_TAG "simonServer"
// https://www.bilibili.com/video/BV1F841177T1/?spm_id_from=333.337.search-card.all.click

void attachGame() {
    if (process != nullptr && process->isAlive())return;
    pid_t pid;
    try {
        pid = hakka::findPidByPackage(packageName);
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "game pid: %d", pid);
    } catch (hakka::no_process_error &e) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "attachGame error: %s", e.what());
    }
    process = std::make_shared<hakka::Target>(pid, hakka::MemoryMode::MEM_FILE);
}

extern "C"
JNIEXPORT jint JNICALL
Java_fan_akua_hakka_Hakka_edit1(JNIEnv *env, jclass clazz) {
    attachGame();
    auto searcher = hakka::MemorySearcher(process);
    searcher.setMemoryRange(hakka::MemoryRange::OTHER);
    searcher.setPageConfig(false, true);
    searcher.setSearchRange(0, 0xfffffffff);

    auto start = std::chrono::steady_clock::now();
    auto size = searcher.searchValue("2680D;170D", 512);
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "elapsed time: %f s", elapsed_seconds.count());

    auto mySet = searcher.getResults();
    i32 tmp;
    for (const auto &ptr: mySet) {
        process->read(ptr, &tmp, sizeof(tmp));
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "find %d [%llx]", tmp, ptr);
    }
    return (jint) size;
}

// 纯净苍穹改司空震
extern "C"
JNIEXPORT jint JNICALL
Java_fan_akua_hakka_Hakka_edit2(JNIEnv *env, jclass clazz) {
    attachGame();
    auto searcher = hakka::MemorySearcher(process);
    searcher.setMemoryRange(hakka::MemoryRange::OTHER);
    searcher.setPageConfig(true, true);
    searcher.setSearchRange(0, 0xfffffffff);

    auto maps = process->getAllMaps();

    std::vector<std::shared_ptr<hakka::ProcMap>> targetMaps;
    auto filter = [](const std::shared_ptr<hakka::ProcMap> &map) {
        return strstr(map->moduleName(), "dataCache.db");
    };
    std::copy_if(maps.begin(), maps.end(), std::back_inserter(targetMaps), filter);


    auto start = std::chrono::steady_clock::now();
    auto size = searcher.searchValue("2120D;91000D", 9, targetMaps);
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "elapsed time: %f s", elapsed_seconds.count());

    auto mySet = searcher.getResults();
    i32 tmp;
    for (const auto &ptr: mySet) {
        process->read(ptr, &tmp, sizeof(tmp));
        // 苍穹改一块钱
        if (tmp == 2120) {
            tmp = 1;
            process->write(ptr, &tmp, sizeof(tmp));
        } else if (tmp == 91000) {
            tmp = 53730;// 536夏洛特代码 03被动
            process->write(ptr, &tmp, sizeof(tmp));
        }
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "find %d [%llx]", tmp, ptr);
    }
    return (jint) size;
}
//78f3b90804
extern "C"
JNIEXPORT jint JNICALL
Java_fan_akua_hakka_Hakka_edit3(JNIEnv *env, jclass clazz) {
    attachGame();

}

extern "C"
JNIEXPORT jint JNICALL
Java_fan_akua_hakka_Hakka_edit4(JNIEnv *env, jclass clazz) {
    attachGame();

}