cmake_minimum_required(VERSION 3.29)
project(ServerWinsockTCP C)

set(CMAKE_C_STANDARD 17)

add_executable(ServerWinsockTCP main.c)

target_link_libraries(ServerWinsockTCP ws2_32)