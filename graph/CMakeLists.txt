add_library(
    graph
    graph.cpp
    graph.hpp
)

target_link_libraries(graph PUBLIC nlohmann_json exception)

target_include_directories(graph PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

