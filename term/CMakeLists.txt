cmake_minimum_required(VERSION 3.15)
project(term)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(term
        term_iterator.hpp
        term.hpp
        sub.hpp
        test_basic.cpp
        test_boolean.cpp
        test_display.cpp
        test_polinomial.cpp term_iterator.cpp)