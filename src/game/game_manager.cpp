//
// Created by unite on 02.08.2024.
//

#include "game/game_manager.h"
#include "game/const.h"
#include "engine/engine.h"
#include "network/client.h"

namespace splash
{

void GameManager::Begin()
{
	gameSystems_.Begin();
	gameRenderer_.Begin();
}
void GameManager::Update(float dt)
{
	if(!IsSpineLoaded() || !IsTextureLoaded())
	{
		return;
	}

	auto* netClient = GetNetworkClient();
	if(netClient == nullptr)
	{
		playerInputs_[0] = GetPlayerInput();
	}
	else
	{
		// import network inputs
	}

	currentTime_ += dt;
	constexpr auto fixedDt = (float)fixedDeltaTime;
	while (currentTime_ > fixedDt)
	{
		Tick();
		currentTime_ -= fixedDt;
	}

	gameRenderer_.Update(dt);
}
void GameManager::End()
{
	RemoveSystem(this);
}
void GameManager::Tick()
{
	gameSystems_.Tick();
	gameRenderer_.Tick();
}
GameManager::GameManager(): gameRenderer_(&gameSystems_)
{
	AddSystem(this);
}
int GameManager::GetSystemIndex() const
{
	return systemIndex_;
}
void GameManager::SetSystemIndex(int index)
{
	systemIndex_ = index;
}
}