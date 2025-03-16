cmake_minimum_required(VERSION 3.10)

project(kip-stack)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -ggdb")

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(main
               src/main.c
               src/tuntap.c
               src/arp.c
               src/netdev.c
               src/eth.c
               src/utils.c
               )
