cmake_minimum_required(VERSION 3.28)

project(airport_simulation VERSION 1.0.0 LANGUAGES CXX)

add_subdirectory(thirdparty_libs/nlohmann_json)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_BUILD_TYPE "DEBUG")
set(CMAKE_CXX_FLAGS "-pthread")

find_package(OpenCV 4.1 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

# find_package(Qt6 REQUIRED COMPONENTS Widgets)
# qt_standard_project_setup()

add_subdirectory(exception)
add_subdirectory(graph)
add_subdirectory(astar)
add_subdirectory(airplane)
add_subdirectory(airport)
add_subdirectory(graphics)

add_executable(airport_simulation
    main.cpp
)

target_link_libraries(airport_simulation PRIVATE graphics nlohmann_json graph astar airplane airport exception ${OpenCV_LIBRARIES})