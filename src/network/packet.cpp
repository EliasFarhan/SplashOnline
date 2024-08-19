//
// Created by unite on 19.08.2024.
//

#include "network/packet.h"

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
ExitGames::Common::JString& ConfirmFrameSerializer::toString(ExitGames::Common::JString& retStr, bool withTypes) const
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
	//TODO deserialize input
}
short InputSerializer::serialize(nByte* pRetVal) const
{
	if(pRetVal)
	{
		//TODO serialize input
	}
	return inputPacket_.inputSize*sizeof(PlayerInput)+sizeof(int)+sizeof(int);
}
ExitGames::Common::JString& InputSerializer::toString(ExitGames::Common::JString& retStr, bool withTypes) const
{
	return retStr = ExitGames::Common::JString("Input Packet");
}
}