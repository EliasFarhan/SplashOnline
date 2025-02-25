#include "rollback/rollback_manager.h"
#include "game/game_manager.h"
#include "network/packet.h"

#include <limits>

namespace splash
{
static RollbackManager* instance = nullptr;
Checksum<static_cast<int>(BulletChecksumIndex::LENGTH)+static_cast<int>(PlayerChecksumIndex::LENGTH)> RollbackManager::ConfirmLastFrame()
{
	const auto inputs = GetInputs(neko::Max(lastConfirmFrame_, static_cast<uint16_t>(0u)));
	confirmFrameGameSystems_.SetPlayerInput(inputs);
	if(lastConfirmFrame_ > 0)
	{
		confirmFrameGameSystems_.SetPreviousPlayerInput(GetInputs(lastConfirmFrame_-1));
	}
	confirmFrameGameSystems_.Tick();

	lastConfirmFrame_++;
	return confirmFrameGameSystems_.CalculateChecksum();
}
int RollbackManager::GetLastReceivedFrame() const
{
	uint16_t lastReceived = std::numeric_limits<uint16_t >::max();
	for(int playerNumber = 0; playerNumber < MaxPlayerNmb; playerNumber++)
	{
		if(!inputDatas_[playerNumber].isValid) continue;
		lastReceived = neko::Min(inputDatas_[playerNumber].lastReceivedFrame, lastReceived);
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
void RollbackManager::SetInput(uint8_t playerNumber, PlayerInput input, uint16_t currentFrame)
{
	if(!IsValid(playerNumber))
	{
		return;
	}
	auto index = currentFrame;
	const auto inputSize = sixit::guidelines::narrow_cast<uint16_t >(inputs_.size());
	if(index >= inputSize)
	{
		const uint16_t delta = inputSize-index+1u;
		std::array<PlayerInput, MaxPlayerNmb> last{};
		if(!inputs_.empty())
		{
			last = inputs_[inputSize - 1];
		}
		for (uint16_t i = 0; i < delta; i++)
		{
			inputs_.push_back(last);
		}
	}
	inputs_[index][playerNumber] = input;
	if(inputDatas_[playerNumber].lastReceivedFrame < currentFrame)
	{
		inputDatas_[playerNumber].lastReceivedFrame = currentFrame;
		for(uint16_t i = index+1; i < inputSize; i++)
		{
			inputs_[i][playerNumber] = input;
		}
	}
}
PlayerInput RollbackManager::GetInput(int playerNumber, int currentFrame) const
{
	int index = currentFrame;
	if(index >= static_cast<int>(inputs_.size()))
	{
		index = static_cast<int>(inputs_.size())-1;
	}
	return inputs_[index][playerNumber];
}
std::pair<std::array<PlayerInput, MaxPlayerInputNmb>, uint8_t> RollbackManager::GetInputs(uint8_t playerNumber, uint16_t currentFrame) const
{
	std::array<PlayerInput, MaxPlayerInputNmb> inputsTmp{};
	uint16_t size = sixit::guidelines::narrow_cast<uint16_t>(neko::Max(currentFrame - neko::Max(static_cast<int>(lastConfirmFrame_), 0) + 1, 1));
	uint16_t firstFrame = neko::Max(lastConfirmFrame_, static_cast<uint16_t>(0u));
	if(size > MaxPlayerInputNmb)
	{
		size = MaxPlayerInputNmb;
		firstFrame = currentFrame-size+1u;
	}
	for(uint8_t i = 0; i < size; i++)
	{
		const auto index = firstFrame+i;
		inputsTmp[i] = inputs_[index][playerNumber];
	}
	return {inputsTmp, sixit::guidelines::narrow_cast<uint8_t>(size)};
}
std::array<PlayerInput, MaxPlayerNmb> RollbackManager::GetInputs(int currentFrame) const
{
	auto index = currentFrame;
	if(index > static_cast<int>(inputs_.size()))
	{
		index = static_cast<int>(inputs_.size())-1;
	}
	return inputs_[index];
}
void RollbackManager::SetInputs(const InputPacket& packet)
{
	const auto playerNumber = packet.playerNumber;
	const auto currentFrame = packet.frame;
	const auto lastReceiveFrame = inputDatas_[playerNumber].lastReceivedFrame;
	const auto firstFrame = sixit::guidelines::narrow_cast<uint16_t>(currentFrame - packet.inputSize + 1);
	for(uint16_t i = 0; i < packet.inputSize; i++)
	{
		uint16_t tmpFrame = firstFrame+i;
		if(tmpFrame < lastReceiveFrame)
		{
			continue;
		}
		const auto newInput = packet.inputs[i];
		if(!inputs_.empty() && GetInput(playerNumber, tmpFrame) != newInput)
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