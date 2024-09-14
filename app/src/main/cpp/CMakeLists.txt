cmake_minimum_required(VERSION 3.22.1)

project("hakka_jni")

add_subdirectory(core)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../assets/${CMAKE_ANDROID_ARCH_ABI})

add_library(${CMAKE_PROJECT_NAME} SHARED
        hakka-lib.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        hakka::core
        log)