add_library(
    graphics
    graphics.cpp
    graphics.hpp
)

target_link_libraries(graphics PUBLIC airplane graph airport ${OpenCV_LIBRARIES})

target_include_directories(graphics PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

