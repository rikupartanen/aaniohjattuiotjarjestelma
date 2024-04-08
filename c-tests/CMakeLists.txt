cmake_minimum_required(VERSION 3.27)

project(c-tests C)
add_executable(main main.c)
target_compile_options(main PRIVATE -O2 -Wall -Wextra)

project(cpp-tests CXX)
add_executable(main_cpp main.cpp)
