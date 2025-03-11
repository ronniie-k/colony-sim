cmake_minimum_required(VERSION 3.30.3)
project(colony-sim VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()

add_subdirectory(src)
set(CMAKE_SUPPRESS_REGENERATION TRUE)
add_subdirectory(thirdparty/vma)
