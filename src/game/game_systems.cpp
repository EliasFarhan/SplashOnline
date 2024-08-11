//
// Created by unite on 02.08.2024.
//
#include "game/game_systems.h"

namespace splash
{

GameSystems::GameSystems(): playerManager_(this), bulletManager_(this)
{

}

void GameSystems::Begin()
{
	playerManager_.Begin();
}

void GameSystems::Tick()
{
	physicsManager_.Step(fixedDeltaTime);
	playerManager_.Tick();
}

void GameSystems::End()
{
	playerManager_.End();
}

}