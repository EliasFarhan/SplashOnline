//
// Created by unite on 19.08.2024.
//

#include "network/packet.h"

#include "utils/log.h"

#include <fmt/format.h>

namespace splash
{

bool ConfirmFrameSerializer::compare(const ExitGames::Common::CustomTypeBase& other) const
{
	return confirmFramePacket_ == static_cast<const ConfirmFrameSerializer&>(other).confirmFramePacket_;
}
void ConfirmFrameSerializer::duplicate(ExitGames::Common::CustomTypeBase* pRetVal) const
{
	*reinterpret_cast<ConfirmFrameSerializer*>(pRetVal) = *this;
}
void ConfirmFrameSerializer::deserialize(const nByte* pData, short length)
{
	if(length != sizeof(ConfirmFramePacket))
	{
		return;
	}
	confirmFramePacket_.frame = *reinterpret_cast<const int*>(pData+ offsetof(ConfirmFramePacket, frame));
	confirmFramePacket_.checksum = *reinterpret_cast<const std::uint32_t*>(pData+ offsetof(ConfirmFramePacket, checksum));
	for(int i = 0; i < MaxPlayerNmb; i++)
	{
		confirmFramePacket_.input[i] = reinterpret_cast<const PlayerInput*>(pData + offsetof(ConfirmFramePacket, input))[i];
	}
}
short ConfirmFrameSerializer::serialize(nByte* pRetVal) const
{
	if(pRetVal)
	{
		std::memcpy(pRetVal, &confirmFramePacket_, sizeof(ConfirmFramePacket));
	}
	return sizeof(ConfirmFramePacket);
}
ExitGames::Common::JString& ConfirmFrameSerializer::toString(ExitGames::Common::JString& retStr, [[maybe_unused]] bool withTypes) const
{
	return retStr = ExitGames::Common::JString("Confirm Frame Packet");
}
bool InputSerializer::compare(const ExitGames::Common::CustomTypeBase& other) const
{
	return inputPacket_ == static_cast<const InputSerializer&>(other).inputPacket_;
}
void InputSerializer::duplicate(ExitGames::Common::CustomTypeBase* pRetVal) const
{
	*reinterpret_cast<InputSerializer*>(pRetVal) = *this;
}
void InputSerializer::deserialize(const nByte* pData, short length)
{
	const auto inputSize = (int)((length - 2*sizeof(int))/sizeof(PlayerInput));
	const auto packetInputSize = *reinterpret_cast<const int*>(pData+sizeof(int));
	if(inputSize != packetInputSize)
	{
		LogError(fmt::format("Input size is not the same: {} vs {}"));
		return;
	}
	inputPacket_.frame = *reinterpret_cast<const int*>(pData);
	inputPacket_.inputSize = inputSize;
	std::memcpy(inputPacket_.inputs.data(), pData+2*sizeof(int), inputSize*sizeof(PlayerInput));

}
short InputSerializer::serialize(nByte* pRetVal) const
{
	if(pRetVal)
	{
		*reinterpret_cast<int*>(pRetVal) = inputPacket_.frame;
		*reinterpret_cast<int*>(pRetVal+sizeof(int)) = inputPacket_.inputSize;
		std::memcpy(pRetVal+2*sizeof(int), inputPacket_.inputs.data(), inputPacket_.inputSize*sizeof(PlayerInput));
	}
	return (short)(inputPacket_.inputSize*sizeof(PlayerInput)+sizeof(int)+sizeof(int));
}
ExitGames::Common::JString& InputSerializer::toString(ExitGames::Common::JString& retStr, [[maybe_unused]]bool withTypes) const
{
	return retStr = ExitGames::Common::JString("Input Packet");
}
}