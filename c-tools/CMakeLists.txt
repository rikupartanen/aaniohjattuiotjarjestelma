cmake_minimum_required(VERSION 3.27)
project(test)

include_directories(include)

set(CMAKE_BUILD_TYPE Debug)


add_executable(main 
    main.c 
    serial.c 
    layerparser.c 
    hasher.c 
    headergen.c)
