cmake_minimum_required(VERSION 3.25)
project(bigfind C)

set(CMAKE_C_STANDARD 11)

find_package(Java REQUIRED)
find_package(JNI REQUIRED)
if (JNI_FOUND)
    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
endif ()

message (STATUS "dir ${NATIVE_BUILD_DIR}")

include_directories(${JNI_INCLUDE_DIRS} ${NATIVE_BUILD_DIR})
add_library(bigfind SHARED main.c)