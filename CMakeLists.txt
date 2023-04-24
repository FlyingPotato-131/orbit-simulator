cmake_minimum_required(VERSION 3.0)
project(orbit_simulator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -g")

add_executable(${PROJECT_NAME} src/draw.cpp src/glad.c)

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE GL glfw X11 pthread Xrandr Xi dl)