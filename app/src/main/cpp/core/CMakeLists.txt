cmake_minimum_required(VERSION 3.18.4)
project(Hakka)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES "src/*.cc")

add_library(${PROJECT_NAME} ${SOURCES})
add_library(hakka::core ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/include)