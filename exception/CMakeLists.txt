add_library(
    exception
    exception.hpp
    exception.cpp
)

target_include_directories(exception PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

