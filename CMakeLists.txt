cmake_minimum_required (VERSION 3.16)
project(SplashOnline)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#NekoLib
set(NEKOLIB_DIR externals/NekoLib)
add_subdirectory(${NEKOLIB_DIR})


#SDL
set(SDL_DIR externals/SDL)
add_subdirectory(${SDL_DIR})
set_target_properties (uninstall SDL2 SDL2_test SDL2main SDL2-static sdl_headers_copy PROPERTIES FOLDER Externals)
include_directories(${SDL_DIR}/include ${SDL_DIR}/src)

#imgui
set(IMGUI_DIR externals/imgui)
file(GLOB IMGUI_FILES ${IMGUI_DIR}/*.cpp ${IMGUI_DIR}/*.h
        ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp)
add_library(imgui ${IMGUI_FILES})
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends/)
target_link_libraries(imgui PUBLIC SDL2-static)


file(GLOB_RECURSE GAME_FILES src/*.cpp include/*.h)

add_library(SplashLib ${GAME_FILES})
target_link_libraries(SplashLib PUBLIC SDL2-static SDL2main imgui NekoCore)
target_include_directories(SplashLib PUBLIC include/)

file(GLOB MAIN_FILES main/*.cpp)
foreach (MAIN_FILE ${MAIN_FILES})
    get_filename_component(MAIN_NAME ${MAIN_FILE} NAME_WE)
    add_executable(${MAIN_NAME} ${MAIN_FILE})
    target_link_libraries(${MAIN_NAME} PUBLIC SplashLib)
endforeach ()
