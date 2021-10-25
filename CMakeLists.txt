cmake_minimum_required(VERSION 3.20)
project(simple_websocket)

set(CMAKE_CXX_STANDARD 20)

add_library(simple_websocket simple_websocket.hpp)
set_target_properties(simple_websocket PROPERTIES LINKER_LANGUAGE CXX)

include(FetchContent)
include(cmake/poco.cmake)
include(cmake/testing.cmake)