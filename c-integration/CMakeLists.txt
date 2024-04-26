cmake_minimum_required(VERSION 3.27)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

project(c-tests C)

add_subdirectory(libs)

add_executable(main src/main.c)
target_sources(main PRIVATE src/layers/conv2d.c)
target_sources(main PRIVATE src/common.c src/model.c src/weights_map.c)
target_sources(main PRIVATE 
  src/layers/conv1.c
  src/layers/dense.c
  src/layers/toinenkonvoluutio.c
  src/layers/normalisaatio.c  
  src/layers/resizing.c  
  src/layers/maxpooling2d.c
)
target_link_libraries(main PRIVATE ffti)
target_include_directories(main PRIVATE libs include)


target_compile_options(main PRIVATE -Og -g -Wall -Wextra )
# target_link_options(main PRIVATE -Wl,--stack-size=4194304)

project(cpp-tests CXX)
add_executable(main_cpp main.cpp)
