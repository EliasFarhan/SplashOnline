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
target_include_directories(SplashLib PUBLIC include/ externals/include)
#fmod
set(FMOD_DIR externals/Fmod)
target_include_directories(SplashLib PUBLIC ${FMOD_DIR}/fsbank/inc ${FMOD_DIR}/lowlevel/inc ${FMOD_DIR}/studio/inc)
link_directories(${FMOD_DIR}/fsbank/lib ${FMOD_DIR}/lowlevel/lib ${FMOD_DIR}/studio/lib)

if(MSVC)
    target_link_libraries(SplashLib PUBLIC fmodstudio64_vc.lib)
    file(COPY ${FMOD_DIR}/studio/lib/fmodstudio64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    file(COPY ${FMOD_DIR}/lowlevel/lib/fmod64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
elseif(APPLE)
    target_link_libraries(SplashLib PUBLIC libfmodstudio.dylib)

    file(COPY ${FMOD_DIR}/studio/lib/libfmodstudio.dylib DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    file(COPY ${FMOD_DIR}/lowlevel/lib/libfmod.dylib DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
else ()

endif ()


#attach data to SplashLib
file(GLOB_RECURSE data_files "${CMAKE_CURRENT_SOURCE_DIR}/data/*")
foreach(DATA ${data_files})
    get_filename_component(FILE_NAME ${DATA} NAME)
    get_filename_component(PATH_NAME ${DATA} DIRECTORY)
    get_filename_component(EXTENSION ${DATA} EXT)
    file(RELATIVE_PATH PATH_NAME "${PROJECT_SOURCE_DIR}" ${PATH_NAME})
    set(DATA_OUTPUT "${PROJECT_BINARY_DIR}/${PATH_NAME}/${FILE_NAME}")
    message("Data input: ${DATA} data output: ${DATA_OUTPUT} path name: ${PATH_NAME}")
    if(NOT EXISTS "${PROJECT_BINARY_DIR}/${PATH_NAME}")
        file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/${PATH_NAME}")
    endif ()

    add_custom_command(
            OUTPUT ${DATA_OUTPUT}
            DEPENDS ${DATA}
            DEPENDS
            COMMAND ${CMAKE_COMMAND} -E copy ${DATA} "${PROJECT_BINARY_DIR}/${PATH_NAME}/${FILE_NAME}"
    )
    list(APPEND DATA_BINARY_FILES ${DATA_OUTPUT})
endforeach()
add_custom_target(
        SplashData
        DEPENDS ${DATA_FILES} ${DATA_BINARY_FILES})
add_dependencies(SplashLib SplashData)

file(GLOB MAIN_FILES main/*.cpp)
foreach (MAIN_FILE ${MAIN_FILES})
    get_filename_component(MAIN_NAME ${MAIN_FILE} NAME_WE)
    add_executable(${MAIN_NAME} ${MAIN_FILE})
    target_link_libraries(${MAIN_NAME} PUBLIC SplashLib)
endforeach ()
