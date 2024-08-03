cmake_minimum_required (VERSION 3.16)
project(SplashOnline)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(ENABLE_PROFILER "Enable Tracy Profiling" OFF)

#NekoLib
set(NEKOLIB_DIR externals/NekoLib)
add_subdirectory(${NEKOLIB_DIR})

#NekoPhysics
set(NEKOPHYS_DIR externals/NekoPhysics)
add_subdirectory(${NEKOPHYS_DIR})


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

#spine sdl
set(SPINE_DIR externals/spine-runtimes)
file(GLOB_RECURSE SPINE_SRC ${SPINE_DIR}/spine-cpp/spine-cpp/src/*.cpp {SPINE_DIR}/spine-sdl/src/*.cpp)
add_library(spine_sdl STATIC ${SPINE_SRC})
target_include_directories(spine_sdl PUBLIC ${SPINE_DIR}/spine-cpp/spine-cpp/include ${SPINE_DIR}/spine-sdl/src)
target_link_libraries(spine_sdl PUBLIC SDL2-static)

#fmt format
add_compile_definitions(FMT_HEADER_ONLY=1)
set(FMT_DIR externals/fmt)
add_subdirectory(${FMT_DIR})


#tracy profiler
if(ENABLE_PROFILER)
    set(TRACY_DIR externals/tracy)
    add_subdirectory(${TRACY_DIR})
endif (ENABLE_PROFILER)


file(GLOB_RECURSE GAME_FILES src/*.cpp include/*.h)

add_library(SplashLib ${GAME_FILES})
target_link_libraries(SplashLib PUBLIC SDL2-static SDL2main imgui spine_sdl NekoCore NekoPhysics)
if(ENABLE_PROFILER)
    target_compile_definitions(SplashLib PUBLIC TRACY_ENABLE=1)
    target_link_libraries(SplashLib PUBLIC TracyClient)
endif(ENABLE_PROFILER)
target_include_directories(SplashLib PUBLIC include/ externals/include ${FMT_DIR}/include)
#fmod
set(FMOD_DIR externals/Fmod)
target_include_directories(SplashLib PUBLIC ${FMOD_DIR}/fsbank/inc ${FMOD_DIR}/lowlevel/inc ${FMOD_DIR}/studio/inc)
link_directories(${FMOD_DIR}/lowlevel/lib ${FMOD_DIR}/studio/lib)

if(MSVC)
    target_link_libraries(SplashLib PUBLIC fmodstudio64_vc.lib)
    target_compile_options(SplashLib PUBLIC "/utf-8" "/W4")
    file(COPY ${FMOD_DIR}/studio/lib/fmodstudio64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    file(COPY ${FMOD_DIR}/lowlevel/lib/fmod64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
elseif(APPLE)
    target_link_libraries(SplashLib PUBLIC libfmodstudio.dylib)
    target_compile_options(SplashLib PUBLIC "-Wall" "-Wextra" "-Wpedantic")
    file(COPY ${FMOD_DIR}/studio/lib/libfmodstudio.dylib DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    file(COPY ${FMOD_DIR}/lowlevel/lib/libfmod.dylib DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
elseif (UNIX)
    link_directories(${CMAKE_CURRENT_BINARY_DIR}/)
    target_compile_options(SplashLib PUBLIC "-Wall" "-Wextra" "-Wpedantic")
    target_link_libraries(SplashLib PUBLIC libfmodstudio.so libfmod.so)
    file(COPY ${FMOD_DIR}/lowlevel/lib/libfmod.so DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    file(COPY ${FMOD_DIR}/lowlevel/lib/libfmod.so.9 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    file(COPY ${FMOD_DIR}/lowlevel/lib/libfmod.so.9.5 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

endif ()
#neko net
set(NEKONET_DIR externals/NekoNet)
target_include_directories(SplashLib PUBLIC ${NEKONET_DIR}/include)
link_directories(${NEKONET_DIR}/lib/)
if(MSVC)
    target_link_libraries(SplashLib PUBLIC
            debug NekoNet_d.lib
            debug Photon_d.lib
            debug Photon-cpp_vc17_debug_windows_md_x64.lib
            debug Common-cpp_vc17_debug_windows_md_x64.lib
            optimized NekoNet.lib
            optimized Photon.lib
            optimized Photon-cpp_vc17_release_windows_md_x64.lib
            optimized Common-cpp_vc17_release_windows_md_x64.lib
    )
elseif(APPLE)
elseif (UNIX)
endif()


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

option(SPLASH_TEST "Enable unit test" OFF)

if(SPLASH_TEST)
    set(CATCH2_DIR "externals/Catch2/")
    add_subdirectory("${CATCH2_DIR}")
    file(GLOB TEST_FILES tests/*.cpp)
    foreach (TEST_FILE ${TEST_FILES})
        get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_FILE})
        target_link_libraries(${TEST_NAME} PUBLIC SplashLib Catch2::Catch2WithMain)
    endforeach ()

endif()