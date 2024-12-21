add_library(
    airplane
    airplane.cpp
    airplane.hpp
)
target_link_libraries(airplane PUBLIC graph astar airport exception)

target_include_directories(airplane PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

