cmake_minimum_required(VERSION 3.15)
project(maze)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(maze
        maze.h
        path.h
        square.h
        maze.cpp
        solve.cpp)