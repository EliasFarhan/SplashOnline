
#include "network/packet.h"
#include "utils/log.h"


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
	std::memcpy(&confirmFramePacket_, pData, sizeof(ConfirmFramePacket));
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
	const auto inputSize = static_cast<uint8_t>((length - 2*sizeof(int))/sizeof(PlayerInput));
	const auto packetInputSize = *reinterpret_cast<const int*>(pData+sizeof(int));
	if(inputSize != packetInputSize)
	{
		LogError(fmt::format("Input size is not the same: {} vs {}", inputSize, packetInputSize));
		return;
	}
	inputPacket_.frame = *reinterpret_cast<const uint16_t *>(pData);
	inputPacket_.playerNumber = *reinterpret_cast<const uint8_t *>(pData+2*sizeof(int));
	inputPacket_.inputSize = inputSize;
	std::memcpy(inputPacket_.inputs.data(), pData+3*sizeof(int), inputSize*sizeof(PlayerInput));

}
short InputSerializer::serialize(nByte* pRetVal) const
{
	if(pRetVal)
	{
		*reinterpret_cast<uint16_t*>(pRetVal) = inputPacket_.frame;
		*reinterpret_cast<uint8_t*>(pRetVal+sizeof(int)) = inputPacket_.inputSize;
		*reinterpret_cast<uint8_t *>(pRetVal+2*sizeof(int)) = inputPacket_.playerNumber;
		std::memcpy(pRetVal+3*sizeof(int), inputPacket_.inputs.data(), inputPacket_.inputSize*sizeof(PlayerInput));
	}
	return static_cast<short>(inputPacket_.inputSize*sizeof(PlayerInput)+3*sizeof(int));
}
ExitGames::Common::JString& InputSerializer::toString(ExitGames::Common::JString& retStr, [[maybe_unused]]bool withTypes) const
{
	return retStr = ExitGames::Common::JString("Input Packet");
}
bool PingSerializer::compare(const ExitGames::Common::CustomTypeBase& other) const
{
	return pingPacket_.gameStartTime == static_cast<const PingSerializer&>(other).pingPacket_.gameStartTime;
}
void PingSerializer::duplicate(ExitGames::Common::CustomTypeBase* pRetVal) const
{
	reinterpret_cast<PingSerializer*>(pRetVal)->pingPacket_.gameStartTime = pingPacket_.gameStartTime;
}
using PingFixed8 = neko::Fixed<uint8_t, 6, uint16_t>;
void PingSerializer::deserialize(const nByte* pData, short length)
{
	if(length != 1)
	{
		//TODO exception?
		return;
	}
	PingFixed8 value = *reinterpret_cast<const PingFixed8*>(pData);
	pingPacket_.gameStartTime = static_cast<float>(value);
}
short PingSerializer::serialize(nByte* pRetVal) const
{
	if(pRetVal)
	{
		const auto value = static_cast<PingFixed8>(pingPacket_.gameStartTime);
		*pRetVal = std::bit_cast<nByte>(value);
	}
	return 1;
}
ExitGames::Common::JString& PingSerializer::toString(ExitGames::Common::JString& retStr, [[maybe_unused]]bool withTypes) const
{
	return retStr = ExitGames::Common::JString("Ping Packet");
}
}