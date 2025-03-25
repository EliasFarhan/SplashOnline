cmake_minimum_required (VERSION 3.16)
project(SplashOnline)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

option(ENABLE_PROFILING "Enable Tracy Profiling" OFF)
option(BUILD_SHARED_LIBS "Build the library as a shared library" OFF)
option(USE_SQLITE "Use sqlite lib" ON)
option(ENABLE_DESYNC_DEBUG "Register states in sqlite3 for debug" OFF)
option(ENABLE_ASAN "Enable Adress Sanitizer" OFF)


#tracy profiler
if(ENABLE_PROFILING)
    set(Tracy_DIR externals/tracy)
    add_subdirectory(${Tracy_DIR})
endif (ENABLE_PROFILING)

if(NOT USE_SQLITE)
    set(ENABLE_DESYNC_DEBUG OFF)
endif ()
#NekoLib
set(NEKOLIB_DIR externals/NekoLib)
add_subdirectory(${NEKOLIB_DIR})

#NekoPhysics
set(NEKOPHYS_DIR externals/NekoPhysics)
add_subdirectory(${NEKOPHYS_DIR})


#SDL
set(SDL_DIR externals/SDL)
add_subdirectory(${SDL_DIR})
#set_target_properties (uninstall SDL2 SDL2_test SDL2main SDL2-static sdl_headers_copy PROPERTIES FOLDER Externals)
include_directories(${SDL_DIR}/include ${SDL_DIR}/src)

#SDL ttf
set(SDL_TTF_DIR externals/SDL_ttf)
set(SDL2TTF_VENDORED ON  CACHE INTERNAL "Use vendor")
add_subdirectory(${SDL_TTF_DIR})
include_directories(${SDL_TTF_DIR})

#imgui
set(IMGUI_DIR externals/imgui)
file(GLOB IMGUI_FILES ${IMGUI_DIR}/*.cpp ${IMGUI_DIR}/*.h
        ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp)
add_library(imgui ${IMGUI_FILES})
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends/)
target_link_libraries(imgui PUBLIC SDL2-static)

#spine sdl
set(SPINE_DIR externals/spine-runtimes)
file(GLOB_RECURSE SPINE_SRC ${SPINE_DIR}/spine-cpp/spine-cpp/src/*.cpp ${SPINE_DIR}/spine-sdl/src/*.cpp)
add_library(spine_sdl STATIC ${SPINE_SRC})
target_include_directories(spine_sdl PUBLIC ${SPINE_DIR}/spine-cpp/spine-cpp/include ${SPINE_DIR}/spine-sdl/src)
target_link_libraries(spine_sdl PUBLIC SDL2-static)

#fmt format
add_compile_definitions(FMT_HEADER_ONLY=1)
set(FMT_DIR externals/fmt)
add_subdirectory(${FMT_DIR})

if(USE_SQLITE)
    set(SQLITE3_DIR "externals/sqlite3")
    add_subdirectory(${SQLITE3_DIR})
endif()
file(GLOB_RECURSE GAME_FILES src/*.cpp include/*.h)

add_library(SplashLib ${GAME_FILES})
target_link_libraries(SplashLib PUBLIC SDL2-static SDL2_ttf SDL2main imgui spine_sdl NekoCore NekoPhysics)
if(USE_SQLITE)
    target_link_libraries(SplashLib PUBLIC sqlite3)
    target_compile_definitions(SplashLib PUBLIC USE_SQLITE)
endif ()
if(ENABLE_DESYNC_DEBUG)
    target_compile_definitions(SplashLib PUBLIC ENABLE_DESYNC_DEBUG)
endif(ENABLE_DESYNC_DEBUG)
if(ENABLE_PROFILING)
    target_compile_definitions(SplashLib PUBLIC TRACY_ENABLE=1)
    target_link_libraries(SplashLib PUBLIC TracyClient)
endif(ENABLE_PROFILING)
target_include_directories(SplashLib PUBLIC include/ externals/include ${FMT_DIR}/include)
#fmod
set(FMOD_DIR externals/Fmod)
target_include_directories(SplashLib PUBLIC ${FMOD_DIR}/fsbank/inc ${FMOD_DIR}/core/inc ${FMOD_DIR}/studio/inc)
link_directories(${FMOD_DIR}/core/lib ${FMOD_DIR}/studio/lib)

if(MSVC)
    target_link_libraries(SplashLib PUBLIC fmodstudio_vc.lib)
    target_link_libraries(SplashLib PUBLIC fmod_vc.lib)
    target_compile_options(SplashLib PUBLIC "/utf-8" "/W4" "/WX" "/permissive-")
    file(COPY ${FMOD_DIR}/studio/lib/fmodstudio.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    file(COPY ${FMOD_DIR}/core/lib/fmod.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    if(ENABLE_ASAN)
        target_compile_options(SplashLib PUBLIC "/fsanitize=address")
    endif (ENABLE_ASAN)
elseif(APPLE)
    target_link_libraries(SplashLib PUBLIC libfmodstudio.dylib)
    target_compile_options(SplashLib PUBLIC "-Wall" "-Wextra" "-Wpedantic")
    file(COPY ${FMOD_DIR}/studio/lib/libfmodstudio.dylib DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    file(COPY ${FMOD_DIR}/core/lib/libfmod.dylib DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
elseif (UNIX)
    link_directories(${CMAKE_CURRENT_BINARY_DIR}/)
    target_compile_options(SplashLib PUBLIC "-Wall" "-Wextra" "-Wpedantic" "-Wshadow" "-Wnon-virtual-dtor"
            "-Wold-style-cast" "-Wunused" "-Werror")
    target_link_libraries(SplashLib PUBLIC libfmodstudio.so libfmod.so)
    file(COPY ${FMOD_DIR}/core/lib/libfmod.so DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    file(COPY ${FMOD_DIR}/core/lib/libfmod.so.13 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    file(COPY ${FMOD_DIR}/core/lib/libfmod.so.13.22 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

endif ()
#neko net
set(NEKONET_DIR externals/NekoNet)
target_include_directories(SplashLib PUBLIC ${NEKONET_DIR}/include)

if(MSVC)
    link_directories(${NEKONET_DIR}/lib/windows/)
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
    link_directories(${NEKONET_DIR}/lib/macosx/)
    target_compile_definitions(SplashLib PUBLIC _EG_IMAC_PLATFORM=1)
    find_library(corefoundation_lib CoreFoundation)
    find_library(cfnetwork_lib CFNetwork)
    target_link_libraries(SplashLib PUBLIC
        optimized "libNekoNet.a"
        debug "libNekoNet_d.a"
        optimized "libPhoton.a"
        debug "libPhoton_d.a"
        optimized "libPhoton-cpp_release_macosx.a"
        debug "libPhoton-cpp_debug_macosx.a"
        optimized "libCommon-cpp_release_macosx.a"
        debug "libCommon-cpp_debug_macosx.a"
        optimized "libcrypto_release_macosx.a"
        debug "libcrypto_debug_macosx.a"
        "-framework Foundation" "-framework CoreFoundation" "-framework Security" "-framework CFNetwork"
    )
elseif (UNIX)
    link_directories(${NEKONET_DIR}/lib/linux/)
    target_compile_definitions(SplashLib PUBLIC _EG_LINUX_PLATFORM=1)
    target_link_libraries(SplashLib PUBLIC
            debug libNekoNet_d.a
            optimized libNekoNet.a
            debug libPhoton_d.a
            optimized libPhoton.a
            debug libPhotonDebug64.a
            optimized libPhotonRelease64.a
            debug libCommonDebug64.a # libCommonDebug64.a
            optimized libCommonRelease64.a #libCommonRelease64.a
    )
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

add_subdirectory(externals/argh)

file(GLOB MAIN_FILES main/*.cpp)
foreach (MAIN_FILE ${MAIN_FILES})
    get_filename_component(MAIN_NAME ${MAIN_FILE} NAME_WE)
    add_executable(${MAIN_NAME} ${MAIN_FILE})
    target_link_libraries(${MAIN_NAME} PUBLIC SplashLib argh)
endforeach ()

#catch2
set(CATCH_PATH "externals/Catch2")
add_subdirectory(${CATCH_PATH})


file(GLOB TEST_FILES test/*.cpp)
add_executable(SplashTest ${TEST_FILES})
target_link_libraries(SplashTest PUBLIC SplashLib Catch2WithMain)