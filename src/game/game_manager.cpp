//
// Created by unite on 02.08.2024.
//

#include "game/game_manager.h"
#include "game/const.h"

namespace splash
{

void GameManager::Begin()
{

}
void GameManager::Update(float dt)
{
	currentTime_ += dt;
	constexpr auto fixedDt = (float)fixedDeltaTime;
	while (currentTime_ > fixedDt)
	{
		Tick();
		currentTime_ -= fixedDt;
	}
}
void GameManager::End()
{

}
void GameManager::Tick()
{

}
}