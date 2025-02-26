
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
	const auto inputSize = static_cast<uint8_t>((length - (sizeof(uint16_t)+2*sizeof(uint8_t)))/sizeof(PlayerInput));
	const auto packetInputSize = *(pData+sizeof(uint16_t));
	if(inputSize != packetInputSize)
	{
		LogError(fmt::format("Input size is not the same: {} vs {}", inputSize, packetInputSize));
		return;
	}
	inputPacket_.frame = *reinterpret_cast<const uint16_t *>(pData);
	inputPacket_.playerNumber = *(pData+sizeof(uint16_t)+sizeof(uint8_t));
	inputPacket_.inputSize = inputSize;
	std::memcpy(inputPacket_.inputs.data(), pData+sizeof(uint16_t)+2*sizeof(uint8_t), inputSize*sizeof(PlayerInput));

}
short InputSerializer::serialize(nByte* pRetVal) const
{
	if(pRetVal)
	{
		*reinterpret_cast<uint16_t*>(pRetVal) = inputPacket_.frame;
		*(pRetVal+sizeof(uint16_t)) = inputPacket_.inputSize;
		*(pRetVal+sizeof(uint16_t)+sizeof(uint8_t)) = inputPacket_.playerNumber;
		std::memcpy(pRetVal+sizeof(uint16_t)+2*sizeof(uint8_t), inputPacket_.inputs.data(), inputPacket_.inputSize*sizeof(PlayerInput));
	}
	return static_cast<short>(inputPacket_.inputSize*sizeof(PlayerInput)+sizeof(uint16_t)+2*sizeof(uint8_t));
}
ExitGames::Common::JString& InputSerializer::toString(ExitGames::Common::JString& retStr, [[maybe_unused]]bool withTypes) const
{
	return retStr = ExitGames::Common::JString("Input Packet");
}

bool PingSerializer::compare(const ExitGames::Common::CustomTypeBase& other) const
{
	return pingPacket_ == static_cast<const PingSerializer&>(other).pingPacket_;
}

void PingSerializer::duplicate(ExitGames::Common::CustomTypeBase* pRetVal) const
{
	*reinterpret_cast<PingSerializer*>(pRetVal) = *this;
}

void PingSerializer::deserialize(const nByte* pData, short length)
{
	if(length != 1)
	{
		return;
	}
	std::memcpy(&pingPacket_.masterTime, pData, 1);
}
short PingSerializer::serialize(nByte* pRetVal) const
{
	if(pRetVal)
	{
		*reinterpret_cast<uint8_t*>(pRetVal) = pingPacket_.masterTime.underlyingValue();
	}
	return static_cast<short>(1);
}

ExitGames::Common::JString& PingSerializer::toString(ExitGames::Common::JString& retStr, [[maybe_unused]] bool withTypes) const
{
	return retStr = ExitGames::Common::JString("Ping Packet");
}

bool DesyncSerializer::compare(const ExitGames::Common::CustomTypeBase& other) const
{
	return desyncPacket_.checksum == static_cast<const DesyncSerializer&>(other).desyncPacket_.checksum;
}

void DesyncSerializer::duplicate(ExitGames::Common::CustomTypeBase* pRetVal) const
{
	*reinterpret_cast<DesyncSerializer*>(pRetVal) = *this;
}

void DesyncSerializer::deserialize(const nByte* pData, short length)
{
	if(length != sizeof(DesyncPacket))
	{
		return;
	}
	std::memcpy(&desyncPacket_, pData, length);
}

short DesyncSerializer::serialize(nByte* pRetVal) const
{
	if(pRetVal)
	{
		*reinterpret_cast<DesyncPacket*>(pRetVal) = desyncPacket_;
	}
	return sixit::guidelines::narrow_cast<short>(sizeof(DesyncPacket));
}

ExitGames::Common::JString& DesyncSerializer::toString(ExitGames::Common::JString& retStr, [[maybe_unused]] bool withTypes) const
{
	return retStr = ExitGames::Common::JString("Desync Packet");
}
}