cmake_minimum_required(VERSION 3.9)
project(function)

set(CMAKE_CXX_STANDARD 17)

add_executable(Function Function.h main.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -D__GLIBCXX_DEBUG -Wall -Wextra")