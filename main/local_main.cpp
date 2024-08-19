//
// Created by unite on 19.08.2024.
//
#include "engine/engine.h"
#include "game/game_manager.h"

#include <SDL_main.h>



int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	splash::Engine engine;
	splash::GameManager gameManager;
	engine.Run();
	return 0;
}