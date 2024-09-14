cmake_minimum_required(VERSION 3.18.1)

PROJECT(starter)

set(CMAKE_CXX_STANDARD 17)

set(C_FLAGS "-Werror=format -fdata-sections -ffunction-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics")
set(LINKER_FLAGS "-Wl,--hash-style=both")
set(C_FLAGS "${C_FLAGS} -O3 -finline-functions -fvisibility=hidden -fvisibility-inlines-hidden -fno-keep-static-consts -fvisibility=hidden -fvisibility-inlines-hidden")
set(LINKER_FLAGS "${LINKER_FLAGS} -Wl,-exclude-libs,ALL -Wl,--gc-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_FLAGS}")
#
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LINKER_FLAGS}")

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mllvm -fla -mllvm -sub -mllvm -sub_loop=2 -mllvm -bcf -mllvm -bcf_prob=50 -mllvm -sobf -mllvm -split")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mllvm -fla -mllvm -sub -mllvm -sub_loop=2 -mllvm -bcf -mllvm -bcf_prob=50 -mllvm -sobf -mllvm -split")

# 设置可执行文件的输出路径为assets
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_FILE}/../../assets/${CMAKE_ANDROID_ARCH_ABI})
# 隐藏符号表
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)

add_executable(${PROJECT_NAME}
        starter.cc selinux.cc)
