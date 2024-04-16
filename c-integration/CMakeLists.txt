cmake_minimum_required(VERSION 3.27)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

project(c-tests C)

add_subdirectory(libs)

add_executable(main src/main.c)
target_sources(main PRIVATE src/common.c src/model.c)
target_sources(main PRIVATE src/layers/conv1.c src/layers/dense.c src/layers/toinenkonvoluutio.c)
target_link_libraries(main PRIVATE fftr)
target_include_directories(main PRIVATE libs)


target_compile_options(main PRIVATE -O2 -Wall -Wextra)

project(cpp-tests CXX)
add_executable(main_cpp main.cpp)
