//
// Created by unite on 09.07.2024.
//

#include "rollback/rollback_manager.h"
#include "game/game_manager.h"

#include <limits>

namespace splash
{
static RollbackManager* instance = nullptr;
std::uint32_t RollbackManager::ConfirmLastFrame()
{
	const auto inputs = GetInputs(neko::Max(lastConfirmFrame_, 0));
	confirmFrameGameSystems_.SetPlayerInput(inputs);
	confirmFrameGameSystems_.Tick();
	if(lastConfirmFrame_ != -1)
	{
		inputs_.erase(inputs_.cbegin());
	}
	lastConfirmFrame_++;
	return confirmFrameGameSystems_.CalculateChecksum();
}
int RollbackManager::GetLastReceivedFrame() const
{
	int lastReceived = std::numeric_limits<int>::max();
	for(int i = 0; i < MaxPlayerNmb; i++)
	{
		if(!inputDatas_[i].isValid) continue;
		lastReceived = neko::Min(inputDatas_[i].lastReceivedFrame, lastReceived);
	}
	return lastReceived;
}
RollbackManager::RollbackManager(const GameData& gameData)
{
	instance = this;
	inputs_.reserve(50*60*2); //2 min of inputs

	for(int i = 0; i < MaxPlayerNmb; i++)
	{
		inputDatas_[i].isValid = gameData.connectedPlayers[i];
	}
}
void RollbackManager::SetInput(int playerNumber, PlayerInput input, int currentFrame)
{
	if(!IsValid(playerNumber))
	{
		return;
	}
	int index = currentFrame-neko::Max(lastConfirmFrame_, 0);
	if(index >= (int)inputs_.size())
	{
		const auto delta = (int)inputs_.size()-index+1;
		std::array<PlayerInput, MaxPlayerNmb> last{};
		if(!inputs_.empty())
		{
			last = inputs_[inputs_.size() - 1];
		}
		for (int i = 0; i < delta; i++)
		{
			inputs_.push_back(last);
		}
	}
	inputs_[index][playerNumber] = input;
	if(inputDatas_[playerNumber].lastReceivedFrame < currentFrame)
	{
		inputDatas_[playerNumber].lastReceivedFrame = currentFrame;
		for(std::size_t i = index+1; i < inputs_.size(); i++)
		{
			inputs_[i][playerNumber] = input;
		}
	}
}
PlayerInput RollbackManager::GetInput(int playerNumber, int currentFrame) const
{
	int index = currentFrame-neko::Max(lastConfirmFrame_, 0);
	if(index >= (int)inputs_.size())
	{
		index = (int)inputs_.size()-1;
	}
	return inputs_[index][playerNumber];
}
std::pair<std::array<PlayerInput, MaxPlayerInputNmb>, int> RollbackManager::GetInputs(int playerNumber, int currentFrame) const
{
	std::array<PlayerInput, MaxPlayerInputNmb> inputsTmp{};
	auto size = neko::Max(currentFrame-neko::Max(lastConfirmFrame_, 0)+1, 1);
	if(size > MaxPlayerInputNmb)
	{
		size = MaxPlayerInputNmb;
	}
	const auto firstFrame = currentFrame-size+1;
	for(int i = 0; i < size; i++)
	{
		const auto index = firstFrame+i;
		inputsTmp[i] = inputs_[index][playerNumber];
	}
	return {inputsTmp, size};
}
std::array<PlayerInput, MaxPlayerNmb> RollbackManager::GetInputs(int currentFrame) const
{
	std::array<PlayerInput, MaxPlayerNmb> inputsTmp{};
	auto index = currentFrame-neko::Max(lastConfirmFrame_, 0);
	if(index > (int)inputs_.size())
	{
		index = (int)inputs_.size()-1;
	}
	for(int i = 0; i < MaxPlayerNmb; i++)
	{
		inputsTmp[i] = inputs_[index][i];
	}
	return inputsTmp;
}
void RollbackManager::SetInputs(const InputPacket& packet)
{
	const auto playerNumber = packet.playerNumber;
	const auto currentFrame = packet.frame;
	const auto lastReceiveFrame = inputDatas_[playerNumber].lastReceivedFrame;
	const auto firstFrame = currentFrame-packet.inputSize + 1;
	for(int i = 0; i < packet.inputSize; i++)
	{
		int tmpFrame = firstFrame+i;
		if(tmpFrame < lastReceiveFrame)
		{
			continue;
		}
		const auto newInput = packet.inputs[i];
		if(GetInput(playerNumber, tmpFrame) != newInput)
		{
			isDirty_ = true;
		}
		SetInput(playerNumber, newInput, tmpFrame);
	}
}
bool RollbackManager::IsValid(int playerNumber) const
{
	return inputDatas_[playerNumber].isValid;
}
const GameSystems& RollbackManager::GetGameSystems() const
{
	return confirmFrameGameSystems_;
}
void RollbackManager::Begin()
{
	confirmFrameGameSystems_.Begin();
}
void RollbackManager::SetDirty(bool dirty)
{
	isDirty_ = dirty;
}
int RollbackManager::GetLastReceivedFrame(int playerNumber) const
{
	return inputDatas_[playerNumber].lastReceivedFrame;
}
bool IsValid(int playerNumber)
{
	return instance->IsValid(playerNumber);
}
}