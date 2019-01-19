cmake_minimum_required(VERSION 3.9)
project(J)

set(CMAKE_CXX_STANDARD 14)

add_executable(J main.cpp my_function.h)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -D__GLIBCXX_DEBUG -Wall -Wextra")