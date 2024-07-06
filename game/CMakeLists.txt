file(GLOB GAME_FILES src/*.cpp include/*.h)

add_executable(main ${GAME_FILES})
target_link_libraries(main SDL2-static SDL2main imgui)