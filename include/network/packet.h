//
// Created by unite on 19.08.2024.
//

#ifndef SPLASHONLINE_NETWORK_PACKET_H_
#define SPLASHONLINE_NETWORK_PACKET_H_

#include "engine/input_manager.h"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpedantic"
#endif
#include "Common-cpp/inc/CustomType.h"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include "game/const.h"

namespace splash
{

enum class PacketType : nByte
{
	START_GAME = 1,
	INPUT,
	CONFIRM_FRAME,
	PING
};

struct ConfirmFramePacket
{
	std::uint32_t checksum{};
	short frame = -1;
	std::array<PlayerInput, MaxPlayerNmb> input{};

	bool operator==(const ConfirmFramePacket& confirmFrame) const
	{
		return frame == confirmFrame.frame && checksum == confirmFrame.checksum && input == confirmFrame.input;
	}
};

class ConfirmFrameSerializer : public ExitGames::Common::CustomType<ConfirmFrameSerializer, static_cast<nByte>(PacketType::CONFIRM_FRAME)>
{
public:
	ConfirmFrameSerializer() = default;
	explicit ConfirmFrameSerializer(const ConfirmFramePacket& confirmFramePacket): confirmFramePacket_(confirmFramePacket){}
	[[nodiscard]] bool compare(const CustomTypeBase& other) const override;
	void duplicate(CustomTypeBase* pRetVal) const override;
	void deserialize(const nByte* pData, short length) override;
	short serialize(nByte* pRetVal) const override;
	ExitGames::Common::JString& toString(ExitGames::Common::JString& retStr, bool withTypes) const override;
	[[nodiscard]] const auto& GetConfirmPacket() const {return confirmFramePacket_;}
private:
	ConfirmFramePacket confirmFramePacket_{};
};

struct InputPacket
{
	std::array<PlayerInput, MaxPlayerInputNmb> inputs{};
	uint16_t frame : 13 = 0u;
	uint8_t playerNumber : 3 = 0u;
	uint8_t inputSize = 0;

	bool operator==(const InputPacket& inputPacket) const
	{
		bool result = frame == inputPacket.frame && inputSize == inputPacket.inputSize && playerNumber == inputPacket.playerNumber;
		if (!result)
			return false;
		for(uint8_t i = 0u; i < inputSize; i++)
		{
			if(inputs[i] != inputPacket.inputs[i])
				return false;
		}
		return true;
	}
};

class InputSerializer : public ExitGames::Common::CustomType<InputSerializer, static_cast<nByte>(PacketType::INPUT)>
{
public:
	InputSerializer() = default;
	explicit InputSerializer(const InputPacket& inputPacket): inputPacket_(inputPacket){}
	[[nodiscard]] bool compare(const CustomTypeBase& other) const override;
	void duplicate(CustomTypeBase* pRetVal) const override;
	void deserialize(const nByte* pData, short length) override;
	short serialize(nByte* pRetVal) const override;
	ExitGames::Common::JString& toString(ExitGames::Common::JString& retStr, bool withTypes) const override;
	[[nodiscard]] const auto& GetPlayerInput() const {return inputPacket_;}
private:
	InputPacket inputPacket_{};
};

struct PingPacket
{
	float gameStartTime = -1.0f;
};

class PingSerializer : public ExitGames::Common::CustomType<PingSerializer, static_cast<nByte>(PacketType::PING)>
{
public:
	bool compare(const CustomTypeBase& other) const override;
	void duplicate(CustomTypeBase* pRetVal) const override;
	void deserialize(const nByte* pData, short length) override;
	short serialize(nByte* pRetVal) const override;
	ExitGames::Common::JString& toString(ExitGames::Common::JString& retStr, bool withTypes) const override;
private:
	PingPacket pingPacket_{};
};
}

#endif //SPLASHONLINE_NETWORK_PACKET_H_
