cmake_minimum_required(VERSION 3.15)
project(SO2_Project)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -lncurses" )

add_executable(SO2_Project main.cpp)