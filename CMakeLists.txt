cmake_minimum_required(VERSION 3.5)
project(shareMemory)

set(CMAKE_CXX_STANDARD 11)

add_executable(shareMemory_put shmfifo.h shmfifo.cpp put.cpp)
add_executable(shareMemory_get get.cpp shmfifo.h shmfifo.cpp)
