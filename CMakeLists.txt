cmake_minimum_required(VERSION 3.21)
project(untitled)

set(CMAKE_CXX_STANDARD 14)

add_executable(untitled main.cpp threadPool.cpp threadPool.h dependencyGraph.cpp dependencyGraph.h)
TARGET_LINK_LIBRARIES(untitled PUBLIC pthread)