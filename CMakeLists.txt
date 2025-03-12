cmake_minimum_required(VERSION 3.10)

project(kip-stack)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -ggdb")

add_executable(main
               src/tuntap.c)

