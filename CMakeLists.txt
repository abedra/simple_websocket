cmake_minimum_required(VERSION 3.20)
project(simple_websocket)

set(CMAKE_CXX_STANDARD 20)

add_library(simple_websocket include/simple_websocket.hpp)
set_target_properties(simple_websocket PROPERTIES LINKER_LANGUAGE CXX)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_SHALLOW    TRUE
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.13.6)
FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/contrib)
enable_testing()
include_directories(include)
add_executable(simple_websocket_test test/SimpleWebSocketTests.cpp)
target_link_libraries(simple_websocket_test Catch2::Catch2)
include(CTest)
include(Catch)
catch_discover_tests(simple_websocket_test)