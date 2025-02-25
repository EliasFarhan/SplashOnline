//
// Created by unite on 19.08.2024.
//
#include "engine/engine.h"
#include "game/game_manager.h"
#include "utils/game_db.h"

#include <SDL_main.h>




int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	splash::AddAudio();
	splash::GameData gameData{};
	gameData.introDelay = 0.1f;
	gameData.period = neko::Scalar {60.0f};
	gameData.connectedPlayers[0] = true;
	gameData.connectedPlayers[1] = true;
	gameData.connectedPlayers[2] = true;
	gameData.connectedPlayers[3] = true;



	splash::GameManager gameManager{gameData};
	splash::RunEngine();
	return 0;
}