//
// Created by unite on 02.08.2024.
//

#include "game/game_manager.h"
#include "game/const.h"
#include "engine/engine.h"
#include "network/client.h"
#include "audio/sound_manager.h"

namespace splash
{

void GameManager::Begin()
{
	gameSystems_.Begin();
	gameRenderer_.Begin();
}
void GameManager::Update(float dt)
{
	if(!IsSpineLoaded() || !IsTextureLoaded() || !IsFmodLoaded())
	{
		return;
	}


	auto* netClient = GetNetworkClient();
	if(netClient == nullptr)
	{
		playerInputs_[0] = GetPlayerInput();
		gameSystems_.SetPlayerInput(playerInputs_);
	}
	else
	{
		const auto playerIndex = netClient->GetPlayerIndex();
		if(playerIndex == 0)
		{
			//We are not in game
			return;
		}
		playerInputs_[playerIndex-1] = GetPlayerInput();
		//TODO import network inputs and confirm frames
	}

	if(!introDelayTimer_.Over())
	{
		int previousTime = (int)introDelayTimer_.RemainingTime();
		introDelayTimer_.Update(dt);
		int currentTime = introDelayTimer_.RemainingTime();
		if((int)previousTime != (int)currentTime)
		{
			if(previousTime != 0)
			{
				auto soundEvent = GetGameSoundEvent((GameSoundId)((int)GameSoundId::VOICE5+(5-previousTime)));
				FmodPlaySound(soundEvent);
			}
		}
		//TODO play sound 5, 4, 3, 2, 1
		if(introDelayTimer_.Over())
		{
			FmodPlaySound(GetGameSoundEvent(GameSoundId::BLAST));
		}
	}

	if(introDelayTimer_.Over())
	{
		currentTime_ += dt;
		constexpr auto fixedDt = (float)fixedDeltaTime;
		while (currentTime_ > fixedDt)
		{
			Tick();
			currentTime_ -= fixedDt;
		}
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
GameManager::GameManager(const GameData& gameData): gameRenderer_(&gameSystems_), introDelayTimer_{gameData.introDelay, gameData.introDelay}
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