#include <unistd.h>
#include <dlfcn.h>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mount.h>
#include "selinux.h"

char *apk_path = nullptr;
char *server_name = nullptr;
char *class_name = nullptr;

int is_num(const char *s) {
    size_t len = strlen(s);
    for (size_t i = 0; i < len; ++i)
        if (s[i] < '0' || s[i] > '9')
            return 0;
    return 1;
}

ssize_t fdgets(char *buf, const size_t size, int fd) {
    ssize_t len = 0;
    buf[0] = '\0';
    while (len < size - 1) {
        ssize_t ret = read(fd, buf + len, 1);
        if (ret < 0)
            return -1;
        if (ret == 0)
            break;
        if (buf[len] == '\0' || buf[len++] == '\n') {
            buf[len] = '\0';
            break;
        }
    }
    buf[len] = '\0';
    buf[size - 1] = '\0';
    return len;
}
//int useNS(int i) {
//    char buf[32];
//    int fileP;
//    int bak;
//
//    sprintf(buf, "/proc/%d/ns/mnt", i);
//    bak = -1;
//    if (access(buf, R_OK) != -1) {
//        fileP = __open_2(buf, 0);
//        if (fileP >= 0) {
//            bak = setns(fileP, 0);
//            close(fileP);
//        }
//    }
//    return bak;
//}


/**
 * app_process启动进程
 * app_process [vm-options] cmd-dir [options] start-class-process_name [main-options]
 * vm-options – VM 选项
 * cmd-dir –父目录 (/system/bin)
 * options –运行的参数 :
 *     –zygote
 *     –start-system-server
 *     –application (api>=14)
 *     --nice-name=xxx (api>=14)
 * start-class-process_name –包含main方法的主类
 * main-options –启动时候传递到main方法中的参数
 * @param value
 */
__attribute__((visibility("hidden")))
void createDemon(char *value, char *process_name, char *class_path) {
    char **args;
    char vm_options[128];
    char options[128];

    printf("setenv\n");
    if (setenv("CLASSPATH", value, true)) {
        printf("Can't setenv\n");
        exit(1);
    }

    args = (char **) malloc(sizeof(char *) * 6);
    sprintf(vm_options, "-Djava.class.path=%s", value);
    args[0] = (char *) "/system/bin/app_process";
    args[1] = vm_options;
    args[2] = (char *) "/system/bin";
    sprintf(options, "--nice-name=%s", process_name);
    args[3] = options;
    args[4] = class_path;
    args[5] = nullptr;
    printf("args[0] %s\n", args[0]);
    printf("args[1] %s\n", args[1]);
    printf("args[2] %s\n", args[2]);
    printf("args[3] %s\n", args[3]);
    printf("args[4] %s\n", args[4]);
    fflush(stdout);
    if (daemon(false, false)) {
        printf("%s Can't create daemon\n", "Failure:");
        exit(1);
    }

    if (execvp((const char *)args[0], args))
        exit(2);
}


inline void foreach_proc(void (*pFunction)(__pid_t)) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir("/proc")))
        return;

    while ((entry = readdir(dir))) {
        if (entry->d_type != DT_DIR) continue;
        if (!is_num(entry->d_name)) continue;
        pid_t pid = atoi(entry->d_name);
        pFunction(pid);
    }

    closedir(dir);
}
int get_proc_name(int pid, char *name, size_t _size) {
    int fd;
    ssize_t procSize;

    char buf[1024];
    snprintf(buf, sizeof(buf), "/proc/%d/cmdline", pid);
    if (access(buf, R_OK) == -1 || (fd = open(buf, O_RDONLY)) == -1)
        return 1;
    if ((procSize = fdgets(buf, sizeof(buf), fd)) == 0) {
        snprintf(buf, sizeof(buf), "/proc/%d/comm", pid);
        close(fd);
        if (access(buf, R_OK) == -1 || (fd = open(buf, O_RDONLY)) == -1)
            return 1;
        procSize = fdgets(buf, sizeof(buf), fd);
    }
    close(fd);

    if (procSize < _size) {
        strncpy(name, buf, static_cast<size_t>(procSize));
        name[procSize] = '\0';
    } else {
        strncpy(name, buf, _size);
        name[_size] = '\0';
    }

    return 0;
}

static int check_selinux(const char *s, const char *t, const char *c, const char *p) {
    int res = se::selinux_check_access(s, t, c, p, nullptr);
    if (res != 0) {
        printf("info: selinux_check_access %s %s %s %s: %d\n", s, t, c, p, res);
        fflush(stdout);
    }
    return res;
}

/**
 * exit 0 主进程正常退出
 * exit 1 主进程异常退出
 * exit 2 demon进程异常退出
 */

int __attribute__ ((visibility ("default"))) main(int argc, char *argv[]) {
    char path[64] = "/data/local/tmp/";
    char *context = nullptr;
    printf("%d arg\n", argc);
    for (int i = 0; i < argc; ++i) {
        printf("%d %s\n", i, argv[i]);
    }
    if (argc != 4) {
        printf("no enough arguments\n");
        fflush(stdout);
        exit(1);
    }
    apk_path = argv[1];
    server_name = argv[2];
    class_name = argv[3];

    printf("uid %d\n", getuid());

    if (getuid()) {
        printf("Can't access root\n");
        fflush(stdout);
        exit(1);
    }
    se::init();

    if (se::getcon(&context) == 0) {
        int res = 0;

        res |= check_selinux("u:r:untrusted_app:s0", context, "binder", "call");
        res |= check_selinux("u:r:untrusted_app:s0", context, "binder", "transfer");

        if (res != 0) {
            printf("fatal: the su you are using does not allow app (u:r:untrusted_app:s0) to connect to su (%s) with binder.\n",
                   context);
            exit(1);
        }
        se::freecon(context);
    }

    strcat(path, server_name);

    chown(path, 0x7D0, 0x7D0);
    se::setfilecon(path, "u:object_r:system_server:s0");

    foreach_proc([](pid_t pid) {
        if (pid == getpid()) return;

        char name[1024];
        if (get_proc_name(pid, name, 1024) != 0) return;

        if (strcmp(server_name, name) != 0)
            return;

        if (kill(pid, SIGKILL) == 0)
            printf("info: killed %d (%s)\n", pid, name);
        else if (errno == EPERM) {
            printf("fatal: can't kill %d, please try to stop existing from app first.\n",
                   pid);
            exit(1);
        } else {
            printf("warn: failed to kill %d (%s)\n", pid, name);
        }
    });

    printf("initPidListener\n");
    if (!access(apk_path, R_OK)) {
        se::setcon("u:object_r:system_server:s0");
        createDemon(apk_path, server_name, class_name);
        exit(0);
    }
    printf("Can't access %s\n", apk_path);
}