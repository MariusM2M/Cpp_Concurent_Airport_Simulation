add_library(
    airport
    airport.cpp
    airport.hpp
)
target_link_libraries(airport PUBLIC graph airplane)

target_include_directories(airport PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

