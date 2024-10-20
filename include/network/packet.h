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
	CONFIRM_FRAME
};

struct ConfirmFramePacket
{
	int frame = -1;
	std::array<PlayerInput, MaxPlayerNmb> input{};
	std::uint32_t checksum{};

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
	const auto& GetConfirmPacket() const {return confirmFramePacket_;}
private:
	ConfirmFramePacket confirmFramePacket_{};
};

struct InputPacket
{
	std::array<PlayerInput, MaxPlayerInputNmb> inputs{};
	int inputSize = -1;
	int frame = -1;
	int playerNumber = -1;

	bool operator==(const InputPacket& inputPacket) const
	{
		bool result = frame == inputPacket.frame && inputSize == inputPacket.inputSize && playerNumber == inputPacket.playerNumber;
		if (!result)
			return false;
		for(int i = 0; i < inputSize; i++)
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

}

#endif //SPLASHONLINE_NETWORK_PACKET_H_
