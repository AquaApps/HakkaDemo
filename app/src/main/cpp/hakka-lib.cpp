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
std::vector<std::shared_ptr<hakka::ProcMap>> targetMaps;

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
    auto maps = process->getAllMaps();

    auto filter = [](const std::shared_ptr<hakka::ProcMap> &map) {
        return strstr(map->moduleName(), "dataCache.db");
    };
    std::copy_if(maps.begin(), maps.end(), std::back_inserter(targetMaps), filter);
}

///data/data/com.tencent.tmgp.sgame/files/dataCache.db
extern "C"
JNIEXPORT jint JNICALL
Java_fan_akua_hakka_Hakka_edit1(JNIEnv *env, jclass clazz) {
    attachGame();
    auto searcher = hakka::MemorySearcher(process);
    searcher.setMemoryRange(hakka::MemoryRange::OTHER);
    searcher.setPageConfig(false, true);
    searcher.setSearchRange(0, 0xfffffffff);

    auto start = std::chrono::steady_clock::now();
    auto size = searcher.searchValue("2680D;170D", 512, targetMaps);
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "elapsed time: %f s", elapsed_seconds.count());

    auto mySet = searcher.getResults();
    i32 tmp;
    for (const auto &ptr: mySet) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "find %d [%llx]", tmp, ptr);
        process->read(ptr, &tmp, sizeof(tmp));
        // 破军改一块钱
        if (tmp == 2680) {
            tmp = 1;
            process->write(ptr, &tmp, sizeof(tmp));
        } else if (tmp == 170) {
            // 1100 11%攻速
            // 2200 22%暴击
            // 3300 33%物理吸血
            // 4400 4400法强
            // 5500 55%冷却
            // 6600 6600法力
            // 7700 7700每五秒回蓝
            // 8800 8800物理防御
            // 9900 9900法术防御
            // 1200 1200最大生命
            // 1300 1300每五秒回血
            // 1400 14%移速
            // 1500 1500普攻强度
            // 1 可购买
            tmp = 98765;
            for (int i = 0; i < 13; i++) {
                process->write(ptr + sizeof(tmp) * i, &tmp, sizeof(tmp));
            }
        }
    }
    return (jint) size;
}

// 纯净苍穹改孙策大招
extern "C"
JNIEXPORT jint JNICALL
Java_fan_akua_hakka_Hakka_edit2(JNIEnv *env, jclass clazz) {
    attachGame();
    auto searcher = hakka::MemorySearcher(process);
    searcher.setMemoryRange(hakka::MemoryRange::OTHER);
    searcher.setPageConfig(true, true);
    searcher.setSearchRange(0, 0xfffffffff);

    auto start = std::chrono::steady_clock::now();
    auto size = searcher.searchValue("2120D;91000D", 9, targetMaps);
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "elapsed time: %f s", elapsed_seconds.count());

    auto mySet = searcher.getResults();
    i32 tmp;
    for (const auto &ptr: mySet) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "find %d [%llx]", tmp, ptr);
        process->read(ptr, &tmp, sizeof(tmp));
        // 苍穹改一块钱
        if (tmp == 2120) {
            tmp = 1;
            process->write(ptr, &tmp, sizeof(tmp));
        } else if (tmp == 91000) {
            tmp = 51030;// 510孙策代码 30大招
            process->write(ptr, &tmp, sizeof(tmp));
        }
    }
    return (jint) size;
}

extern "C"
JNIEXPORT jint JNICALL
Java_fan_akua_hakka_Hakka_edit3(JNIEnv *env, jclass clazz) {
    attachGame();

}
// 3150 115
extern "C"
JNIEXPORT jint JNICALL
Java_fan_akua_hakka_Hakka_edit4(JNIEnv *env, jclass clazz) {
    attachGame();

}


extern "C"
JNIEXPORT jlong JNICALL
Java_fan_akua_hakka_Hakka_wallHack(JNIEnv *env, jclass clazz) {
    attachGame();
    auto searcher = hakka::MemorySearcher(process);
    searcher.setMemoryRange(hakka::MemoryRange::CA);
    searcher.setPageConfig(true, true);
    searcher.setSearchRange(0, 0xfffffffff);

    auto start = std::chrono::steady_clock::now();

    auto maps = process->getAllMaps();

    std::vector<std::shared_ptr<hakka::ProcMap>> filterMap;
    auto filter = [](const std::shared_ptr<hakka::ProcMap> &map) {
        return strstr(map->moduleName(), "libc_malloc") and map->size() == 0xa00000;
    };
    std::copy_if(maps.begin(), maps.end(), std::back_inserter(filterMap), filter);

    searcher.searchValue("-2147478520D;1D;0D", 12, filterMap);
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "elapsed time: %f s", elapsed_seconds.count());

    //地图(-58900,58900)
    auto mySet = searcher.getResults();
    i32 tmp1, tmp2;
    ptr_t headPtr;
    for (const auto &ptr: mySet) {
        process->read(ptr, &tmp1, sizeof(tmp1));
        process->read(ptr - 0x8, &tmp2, sizeof(tmp2));
        if (tmp1 == -2147478520 and tmp2 == 0) {
            headPtr = ptr;
            break;
        }
    }

//    ptr_t addr = 0;
//    i32 x, y;
//    int offset = 0;
//    while (1) {
//        process->read(headPtr + offset * 0x20 + 0xc, &addr, sizeof(addr));
//        if (addr == 0) {
//            break;
//        }
//        process->read(addr - 0x40, &x, sizeof(x));
//        process->read(addr - 0x40 + 8, &y, sizeof(y));
//        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%d (%d,%d)", offset, x, y);
//        offset++;
//    }
    return headPtr;
}
extern "C"
JNIEXPORT jobject JNICALL
Java_fan_akua_hakka_Hakka_readEntry(JNIEnv *env, jclass clazz, jlong address) {
    jclass entryClass = env->FindClass("fan/akua/hakka/PlayerEntry");

    jmethodID constructor = env->GetMethodID(entryClass, "<init>", "(JJJ)V");

//    jobject entryObject = env->NewObject(entryClass, constructor, x, y, index);
//    return entryObject;
}