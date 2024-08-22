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




	if(!introDelayTimer_.Over())
	{
		int previousTime = (int)introDelayTimer_.RemainingTime();
		introDelayTimer_.Update(dt);
		int currentTime = (int)introDelayTimer_.RemainingTime();
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
			currentFrame_ = 0;
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
	auto* netClient = GetNetworkClient();
	if(netClient == nullptr)
	{
		playerInputs_[0] = GetPlayerInput();
		gameSystems_.SetPlayerInput(playerInputs_);
	}
	else
	{
		const auto playerIndex = netClient->GetPlayerIndex();
		if(playerIndex == -1)
		{
			//We are not in game
			return;
		}
		const auto localPlayerinput = GetPlayerInput();
		playerInputs_[playerIndex-1] = localPlayerinput;
		rollbackManager_.SetInput(playerIndex-1, localPlayerinput, currentFrame_);

		//import network inputs
		auto inputs = netClient->GetInputPackets();
		for(auto& input: inputs)
		{
			rollbackManager_.SetInputs(input);
		}

		//TODO import confirm frames

		//TODO rollback if needed
		if(rollbackManager_.IsDirty())
		{

		}
		playerInputs_ = rollbackManager_.GetInputs(currentFrame_);
		gameSystems_.SetPlayerInput(playerInputs_);
	}
	gameSystems_.Tick();
	gameRenderer_.Tick();

	if(netClient != nullptr)
	{
		//send input
		InputPacket inputPacket{};
		inputPacket.playerNumber = netClient->GetPlayerIndex()-1;
		inputPacket.frame = currentFrame_;
		const auto inputs = rollbackManager_.GetInputs( inputPacket.playerNumber, currentFrame_);
		inputPacket.inputs = inputs.first;
		inputPacket.inputSize = inputs.second;
		netClient->SendInputPacket(inputPacket);

		//TODO validate frame if master
	}
	currentFrame_++;
}
GameManager::GameManager(const GameData& gameData):
	rollbackManager_(gameData),
	gameRenderer_(&gameSystems_),
	introDelayTimer_{gameData.introDelay, gameData.introDelay},
	connectedPlayers_(gameData.connectedPlayers)
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