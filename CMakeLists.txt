cmake_minimum_required(VERSION 3.10)
project(Galaga)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML COMPONENTS graphics window system REQUIRED)

add_executable(galaga main.cpp Game.cpp Shapes.cpp)

target_link_libraries(galaga
    sfml-graphics
    sfml-window
    sfml-system
)

target_include_directories(galaga PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
