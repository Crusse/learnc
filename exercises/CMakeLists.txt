cmake_minimum_required(VERSION 2.6)
project(c_tests LANGUAGES C)

add_executable(1-19 1-19.c)
add_executable(1-22 1-22.c)

install(TARGETS 1-19 1-22 RUNTIME DESTINATION bin)
