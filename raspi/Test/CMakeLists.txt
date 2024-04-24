cmake_minimum_required(VERSION 3.5)

project(Model LANGUAGES CXX C)

set(CMAKE_INSTALL_PREFIX /home/root/opt)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ffti STATIC libs/libFFTi/FFTi.c)
target_link_libraries(ffti PUBLIC m)
# FFT lib headers
target_sources(ffti PRIVATE libs/libFFTi/FFTi.h)

add_library(model STATIC src/model.c src/common.c)
target_link_libraries(model PUBLIC m)
target_sources(model PRIVATE
    src/layers/conv1.c
    src/layers/conv2d.c
    src/layers/dense.c
    src/layers/toinenkonvoluutio.c
    src/layers/maxpooling2d.c
    src/layers/normalisaatio.c
    src/layers/resizing.c
    src/weights_map.c)
target_include_directories(model PRIVATE include)
# Model headers
file(GLOB model_headers src/layers/*.h src/model.h src/common.h include/weights_map.h)
target_sources(model PRIVATE ${model_headers})

add_executable(app main.cpp)
target_include_directories(app PRIVATE libs/libFFTi)
target_link_libraries(app PUBLIC model ffti)

# Qt libs
find_package(Qt6 REQUIRED COMPONENTS SerialPort)
target_link_libraries(app PRIVATE Qt6::SerialPort)

include(GNUInstallDirs)
install(TARGETS app
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
