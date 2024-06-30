cmake_minimum_required (VERSION 3.16)
project(SplashOnline)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(SDL_DIR externals/SDL)
add_subdirectory(${SDL_DIR})
set_target_properties (uninstall SDL2 SDL2_test SDL2main SDL2-static sdl_headers_copy PROPERTIES FOLDER Externals)
include_directories(${SDL_DIR}/include ${SDL_DIR}/src)

add_subdirectory(game/)