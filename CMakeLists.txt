link_libraries(ws2_32 wsock32)
cmake_minimum_required(VERSION 3.17)
project(probaServaka C)

set(CMAKE_C_STANDARD 11)

add_executable(probaServaka main.c)