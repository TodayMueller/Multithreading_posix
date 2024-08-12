cmake_minimum_required(VERSION 3.9)
project(lab2)

set(CMAKE_CXX_STANDARD 11)

add_executable(lab2 src/main.cpp src/lab2.cpp src/lab2.h)

target_link_libraries(lab2 pthread)

