cmake_minimum_required(VERSION 3.25)
project(bigfind C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
    pkg_search_module(LIBDIVSUFSORT libdivsufsort)
    if (LIBDIVSUFSORT_FOUND)
        message(STATUS "found ${LIBDIVSUFSORT_FOUND}")
        message(STATUS "libdivsufsort ${LIBDIVSUFSORT_INCLUDE_DIRS}")
        message(STATUS "libdivsufsort ${LIBDIVSUFSORT_LIBRARIES}")
    endif()
endif()

find_package(Java REQUIRED)
find_package(JNI REQUIRED)
if (JNI_FOUND)
    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
endif ()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${NATIVE_BUILD_DIR}/libs)

include_directories(${JNI_INCLUDE_DIRS} ${NATIVE_BUILD_DIR} ${NATIVE_INCLUDE_DIR})
add_library(bigfind SHARED main.c)

target_link_directories(${PROJECT_NAME} PUBLIC ${LIBDIVSUFSORT_INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME} PRIVATE ${LIBDIVSUFSORT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(bigfind ${NATIVE_LIBS_DIR}/libfemto.a ${NATIVE_LIBS_DIR}/libfemto_util.a pthread divsufsort)