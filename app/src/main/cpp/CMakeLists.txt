cmake_minimum_required(VERSION 3.22.1)
project(mobfgsr_capture_stage1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_library(log-lib log)

# Stage 1 intentionally builds without LSFG, DXBC, PE parsing or Vulkan
# frame-generation dependencies. MediaProjection capture remains in Kotlin.
add_library(lsfg-android SHARED
    mobfgsr_stage1_jni.cpp
)
target_link_libraries(lsfg-android PRIVATE ${log-lib})
