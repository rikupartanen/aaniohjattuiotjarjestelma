cmake_minimum_required(VERSION 3.27)

project(Libs)
add_library(fftr STATIC libFFTr/FFTr.c)
target_link_libraries(fftr PRIVATE m)

add_library(ffti STATIC libFFTi/FFTi.c)
target_link_libraries(ffti PRIVATE m)