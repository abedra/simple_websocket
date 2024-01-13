FetchContent_Declare(
        simple_websocket
        GIT_SHALLOW TRUE
        GIT_REPOSITORY https://github.com/abedra/simple_websocket
        GIT_TAG v0.0.10
        CONFIGURE_COMMAND ""
        BUILD_COMMAND "")
FetchContent_GetProperties(simple_websocket)
if(NOT simple_websocket_POPULATED)
    FetchContent_Populate(simple_websocket)
endif()

add_library(simple_websocket INTERFACE)
target_include_directories(simple_websocket INTERFACE ${simple_websocket_SOURCE_DIR})
