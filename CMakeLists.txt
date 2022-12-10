cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_FLAGS "-std=c++17 -rdynamic -fPIC")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 17)

project(tetris)

file(GLOB SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
add_executable(tetris ${SOURCE})
target_link_libraries(tetris pthread ncurses)


