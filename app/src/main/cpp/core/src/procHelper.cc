#include "hakkaHelper.h"
#include "exception.h"

#include <string>
#include <istream>
#include <sstream>
#include <cctype>

template<size_t N>
auto inline readProcFile(pid_t pid, const char *name, char *dest) -> void {
    std::string filePath = "/proc/" + std::to_string(pid) + "/" + name;
    FILE *_fp = fopen(filePath.c_str(), "r");
    if (_fp != nullptr) {
        std::fgets(dest, N, _fp);
        std::fclose(_fp);
    } else {
        dest[0] = '\0';
    }
}


auto hakka::getPidList() -> std::vector<pid_t> {
    std::vector<pid_t> list;
    auto *procDir = opendir("/proc");
    if (procDir == nullptr) {
        throw hakka::file_cannot_open();
    }
    struct dirent *pidFile;
    char stat[256];
    while ((pidFile = readdir(procDir)) != nullptr) {
        if (pidFile->d_type != DT_DIR || std::isdigit(pidFile->d_name[0]) == 0) {
            continue;
        }
        pid_t pid = std::stoi(pidFile->d_name);
        readProcFile<sizeof(stat)>(pid, "stat", stat);
        std::istringstream iss((std::string(stat)));
        std::string token;
        for (int i = 0; i < 3; ++i) {
            iss >> token;
        }
        // Running || Sleeping || Uninterruptible Sleep
        if (token == "R" || token == "S" || token == "D") {
            list.emplace_back(pid);
        }
    }
    closedir(procDir);
    return std::move(list);
}

auto hakka::findPidByPackage(std::string &package) -> pid_t {
    auto *procDir = opendir("/proc");
    if (procDir == nullptr) {
        throw hakka::file_cannot_open();
    }
    struct dirent *pidFile;
    char cmdLine[128];
    while ((pidFile = readdir(procDir)) != nullptr) {
        if (pidFile->d_type != DT_DIR || ((std::isdigit(pidFile->d_name[0])) == 0)) {
            continue;
        }
        pid_t pid = std::stoi(pidFile->d_name);
        readProcFile<sizeof(cmdLine)>(pid, "cmdline", cmdLine);

        if (package == cmdLine) {
            closedir(procDir);
            return pid;
        }
    }
    closedir(procDir);
    throw no_process_error();
}

auto hakka::getProcessList() -> std::vector<hakka::Proc> {
    std::vector<hakka::Proc> list;
    auto *procDir = opendir("/proc");
    if (procDir == nullptr) {
        throw hakka::file_cannot_open();
    }
    struct dirent *pidFile;
    char stat[256];
    while ((pidFile = readdir(procDir)) != nullptr) {
        if (pidFile->d_type != DT_DIR || ((std::isdigit(pidFile->d_name[0])) == 0)) {
            continue;
        }
        hakka::Proc mProc;
        pid_t pid = std::stoi(pidFile->d_name);
        readProcFile<sizeof(stat)>(pid, "stat", stat);
        std::istringstream iss((std::string(stat)));
        iss >> mProc.pid;
        iss >> mProc.comm;
        iss >> mProc.state;
        iss >> mProc.ppid;
        // Running || Sleeping || Uninterruptible Sleep
        if (mProc.state == 'R' || mProc.state == 'S' || mProc.state == 'D') {
            list.push_back(mProc);
        }
    }
    closedir(procDir);
    return std::move(list);
}