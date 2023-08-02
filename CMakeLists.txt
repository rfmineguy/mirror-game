cmake_minimum_required(VERSION 3.20)
project(mirrors LANGUAGES C)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

include(FetchContent)

FetchContent_Declare(raylib GIT_REPOSITORY https://github.com/raysan5/raylib.git GIT_TAG 4.5.0)
FetchContent_MakeAvailable(raylib)

add_library(mirrorlib src/mirror_lib.c)
target_link_libraries(mirrorlib PRIVATE raylib)
target_include_directories(mirrorlib PRIVATE ${raylib_INCLUDE_DIR})

add_executable(mirrors src/main.c)
target_link_libraries(mirrors PRIVATE raylib mirrorlib)
target_include_directories(mirrors PRIVATE ${raylib_INCLUDE_DIR})
