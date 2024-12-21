add_library(
    astar
    astar.cpp
    astar.hpp
)
target_link_libraries(astar PUBLIC graph exception)

target_include_directories(astar PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

