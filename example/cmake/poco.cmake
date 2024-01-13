FetchContent_Declare(
    poco
    GIT_SHALLOW TRUE
    GIT_REPOSITORY "https://github.com/pocoproject/poco.git"
    GIT_TAG        "poco-1.12.5-release"
)
FetchContent_MakeAvailable(poco)
FetchContent_GetProperties(poco)
if (NOT poco_POPULATED)
  FetchContent_Populate(poco)
endif()
