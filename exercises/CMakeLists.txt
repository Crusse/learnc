cmake_minimum_required(VERSION 2.6)
project(c_tests LANGUAGES C)

add_executable(1-19 1-19.c)
add_executable(1-22 1-22.c)
add_executable(2-1 2-1.c)
add_executable(2-3 2-3.c)
add_executable(2-4 2-4.c)
add_executable(2-6 2-6.c)
add_executable(2-9 2-9.c)

install(TARGETS 1-19 1-22 2-1 2-3 2-4 2-6 2-9 RUNTIME DESTINATION bin)
