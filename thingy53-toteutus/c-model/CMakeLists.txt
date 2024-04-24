cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(c-thingy-model)
target_sources(app PRIVATE 
  src/main.c 
  src/qspi/qspi.s 
  src/qspi/qspi.c
)

zephyr_library_named(model)
zephyr_library_sources(
  src/common.c 
  src/model.c
  src/layers/conv1.c 
  src/layers/dense.c
  src/layers/toinenkonvoluutio.c
  src/layers/maxpooling2d.c
  src/layers/normalisaatio.c
  src/layers/resizing.c
  src/weights_map.c
)
zephyr_library_include_directories(include)

zephyr_library_named(fftr)
zephyr_library_sources(libs/libFFTr/FFTr.c)

zephyr_library_named(ffti)
zephyr_library_sources(libs/libFFTi/FFTi.c)

target_link_libraries(app PUBLIC model ffti)
target_include_directories(app PRIVATE libs/libFFTr libs/libFFTi)
