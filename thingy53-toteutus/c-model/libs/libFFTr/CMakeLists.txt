cmake_minimum_required(VERSION 3.20)

# project(fftr)
# add_library(fftr STATIC "")
# target_compile_options(fftr PRIVATE -Wall -Wextra)
# set_target_properties(fftr PROPERTIES C_STANDARD 11)

# target_include_directories(fftr PUBLIC .)
# target_sources(fftr PRIVATE FFTr.c)

# zephyr_append_cmake_library(fftr)
# target_link_libraries(fftr PUBLIC zephyr_interface m)

zephyr_include_directories(.)
zephyr_library_named(fftr)
zephyr_library_sources(FFTr.c)
zephyr_library_link_libraries(m)

