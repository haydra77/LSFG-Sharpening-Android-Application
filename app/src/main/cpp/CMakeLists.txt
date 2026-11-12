cmake_minimum_required(VERSION 3.22.1)
project(mobfgsr_android_blend LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_library(log-lib log)
find_library(android-lib android)

# Android native prototype: AHardwareBuffer CPU read + ANativeWindow output.
# This validates real intermediate-frame presentation before GPU optical flow.
add_library(lsfg-android SHARED
    mobfgsr_stage1_jni.cpp
)
target_link_libraries(lsfg-android PRIVATE ${log-lib} ${android-lib})
