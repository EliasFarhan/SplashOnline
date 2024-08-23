//
// Created by unite on 02.08.2024.
//

#include "game/game_manager.h"
#include "game/const.h"
#include "engine/engine.h"
#include "network/client.h"
#include "audio/sound_manager.h"
#include "utils/log.h"

#include <fmt/format.h>

template <> class fmt::formatter<splash::PlayerInput> {
public:
	constexpr auto parse (format_parse_context& ctx) { return ctx.begin(); }
	template <typename Context>
	constexpr auto format (splash::PlayerInput const& playerInput, Context& ctx) const {
		return format_to(ctx.out(), "({}, {})({},{})",
			(float)playerInput.moveDirX,
			(float)playerInput.moveDirY,
			(float)playerInput.targetDirX,
			(float)playerInput.targetDirY);  // --== KEY LINE ==--
	}
};

namespace splash
{

void GameManager::Begin()
{
	gameSystems_.Begin();
	gameRenderer_.Begin();
	rollbackManager_.Begin();
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
		if(introDelayTimer_.Over())
		{
			currentFrame_ = 0;
			FmodPlaySound(GetGameSoundEvent(GameSoundId::BLAST));
			auto* netClient = GetNetworkClient();
			if(netClient != nullptr && netClient->IsMaster())
			{
				LogDebug("I am master!");
			}
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
	gameSystems_.End();
	gameRenderer_.End();
}
void GameManager::Tick()
{
	auto* netClient = GetNetworkClient();
	PlayerInput localPlayerInput = GetPlayerInput();
	if(netClient == nullptr)
	{
		playerInputs_[0] = localPlayerInput;
		gameSystems_.SetPlayerInput(playerInputs_);
	}
	else
	{
		const auto localPlayerNumber = netClient->GetPlayerIndex()-1;
		LogDebug(fmt::format("Local Input p{} f{} input: {}", localPlayerNumber+1, currentFrame_, localPlayerInput));
		rollbackManager_.SetInput(localPlayerNumber, localPlayerInput, currentFrame_);
		{
			const auto& inputs = rollbackManager_.GetInputs(currentFrame_);
			if(inputs[localPlayerNumber] != localPlayerInput)
			{
				LogError(fmt::format("WTF! Local input: {} Rollback Input: {}", localPlayerInput, inputs[localPlayerNumber]));
			}
		}
		//import network inputs
		auto inputPackets = netClient->GetInputPackets();
		for(auto& inputPacket: inputPackets)
		{
			LogDebug(fmt::format("Received input from p{} f{} s{} with input: {} at f{}",
				inputPacket.playerNumber+1,
				inputPacket.frame,
				inputPacket.inputSize,
				inputPacket.inputs[inputPacket.inputSize-1],
				currentFrame_));
			rollbackManager_.SetInputs(inputPacket);
		}

		//import confirm frames
		auto confirmPackets = netClient->GetConfirmPackets();
		for(auto& confirmPacket : confirmPackets)
		{
			LogDebug(fmt::format("Received confirm inputs f{}: p1: {} p2: {}", confirmPacket.frame, confirmPacket.input[0], confirmPacket.input[1]));
			const auto lastConfirmFrame = confirmPacket.frame;
			if(lastConfirmFrame != rollbackManager_.GetLastConfirmFrame()+1)
			{
				LogError(fmt::format("Not the same Confirm Frame: server {} local {}",
					lastConfirmFrame,
					rollbackManager_.GetLastConfirmFrame()+1));
				std::terminate();
			}
			if(lastConfirmFrame > rollbackManager_.GetLastReceivedFrame())
			{
				LogWarning("Confirm Frame is further than received from unreliable");
			}
			const auto& confirmInputs = confirmPacket.input;
			for(int playerNumber = 0; playerNumber < MaxPlayerNmb; playerNumber++)
			{
				if(!IsValid(playerNumber))
				{
					continue;
				}
				if(rollbackManager_.GetLastReceivedFrame(playerNumber) >= lastConfirmFrame)
				{
					const auto checkInput = rollbackManager_.GetInput(playerNumber, lastConfirmFrame);
					if (confirmInputs[playerNumber] != checkInput)
					{
						LogError(fmt::format("Not the same input for confirm input p{} f{}: remote: {} local: {}",
							playerNumber+1,
							lastConfirmFrame,
							confirmInputs[playerNumber],
							checkInput));
						std::terminate();
					}
				}
				rollbackManager_.SetInput(playerNumber, confirmInputs[playerNumber], lastConfirmFrame);
			}
			const auto lastConfirmValue = confirmPacket.checksum;
			const auto localConfirmValue = rollbackManager_.ConfirmLastFrame();
			if(localConfirmValue != lastConfirmValue)
			{
				LogError("Desync");
			}
		}


		//rollback if needed
		if(rollbackManager_.IsDirty())
		{
			gameSystems_.RollbackFrom(rollbackManager_.GetGameSystems());
			if(rollbackManager_.GetGameSystems().CalculateChecksum() != gameSystems_.CalculateChecksum())
			{
				LogError("Current Frame Game System does not have the same confirm value than Confirm Frame System");
			}
			int firstFrame = neko::Max(rollbackManager_.GetLastConfirmFrame(), 0);
			for(int i = 0; i < currentFrame_-firstFrame; i++)
			{
				const auto rollbackInputs = rollbackManager_.GetInputs(firstFrame+i);
				if(firstFrame+i > 0)
				{
					gameSystems_.SetPreviousPlayerInput(rollbackManager_.GetInputs(firstFrame+i-1));
				}
				gameSystems_.SetPlayerInput(rollbackInputs);
				gameSystems_.Tick();
			}
			rollbackManager_.SetDirty(false);
		}
		playerInputs_ = rollbackManager_.GetInputs(currentFrame_);
		gameSystems_.SetPlayerInput(playerInputs_);
		if(currentFrame_ > 0)
		{
			gameSystems_.SetPreviousPlayerInput(rollbackManager_.GetInputs(currentFrame_-1));
		}
	}
	gameSystems_.Tick();
	gameRenderer_.Tick();

	if(netClient != nullptr)
	{
		//send input
		const auto playerNumber =  netClient->GetPlayerIndex()-1;
		InputPacket inputPacket{};
		inputPacket.playerNumber = playerNumber;
		inputPacket.frame = currentFrame_;
		const auto inputs = rollbackManager_.GetInputs( playerNumber, currentFrame_);
		if(localPlayerInput != inputs.first[inputs.second-1])
		{
			LogError(fmt::format("WTF! Local input: {} Rollback Input: {}", localPlayerInput, inputs.first[inputs.second-1]));
			std::terminate();
		}
		inputPacket.inputs = inputs.first;
		inputPacket.inputSize = inputs.second;
		netClient->SendInputPacket(inputPacket);
		LogDebug(fmt::format("Sent input from p{} f{} with input: {}",
			inputPacket.playerNumber+1,
			currentFrame_,
			inputPacket.inputs[inputPacket.inputSize-1]
			));


		//validate frame
		if(netClient->IsMaster())
		{
			while(rollbackManager_.GetLastReceivedFrame() > neko::Max(rollbackManager_.GetLastConfirmFrame(), 0))
			{
				const auto confirmValue = rollbackManager_.ConfirmLastFrame();
				const auto lastConfirmFrame = rollbackManager_.GetLastConfirmFrame();
				ConfirmFramePacket confirmPacket{};
				confirmPacket.frame = lastConfirmFrame;
				confirmPacket.checksum = confirmValue;
				confirmPacket.input = rollbackManager_.GetInputs(lastConfirmFrame);
				LogDebug(fmt::format("Sending confirm inputs f{} p1: {} p2: {}", lastConfirmFrame, confirmPacket.input[0], confirmPacket.input[1]));
				netClient->SendConfirmFramePacket(confirmPacket);
			}
		}
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