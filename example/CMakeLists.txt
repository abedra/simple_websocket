cmake_minimum_required(VERSION 3.18)
project(example)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
include(cmake/poco.cmake)
include(cmake/simple_websocket.cmake)

include_directories(${PROJECT_SOURCE_DIR}/src)
add_executable(example bin/main.cpp)
target_link_libraries(example Poco::NetSSL simple_websocket)
