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
#include "rollback/rollback_manager.h"

namespace splash
{

enum class PacketType : nByte
{
	START_GAME = 1,
	INPUT,
	CONFIRM_FRAME,
	PING,
	DESYNC
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
	uint16_t frame = 0u;
	uint8_t playerNumber = 0u;
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
    using time_type = neko::Fixed<uint8_t, 5, uint16_t>;
	time_type masterTime;
	bool operator==(const PingPacket& other) const
	{
		return masterTime == other.masterTime;
	}
};

class PingSerializer : public ExitGames::Common::CustomType<PingSerializer, static_cast<nByte>(PacketType::PING)>
{
public:
	PingSerializer() = default;
	explicit PingSerializer(const PingPacket& pingPacket): pingPacket_(pingPacket){}
	bool compare(const CustomTypeBase& other) const override;

	void duplicate(CustomTypeBase* pRetVal) const override;

	void deserialize(const nByte* pData, short length) override;

	short serialize(nByte* pRetVal) const override;

	ExitGames::Common::JString& toString(ExitGames::Common::JString& retStr, bool withTypes) const override;

	const auto& GetPingPacket() const {return pingPacket_;}
private:
	PingPacket pingPacket_{};
};

struct DesyncPacket
{
	RollbackChecksum checksum;
};

class DesyncSerializer : public ExitGames::Common::CustomType<DesyncSerializer, static_cast<nByte>(PacketType::DESYNC)>
{
public:
	DesyncSerializer() = default;
	explicit DesyncSerializer(const DesyncPacket& desyncPacket): desyncPacket_(desyncPacket){}

	bool compare(const CustomTypeBase& other) const override;

	void duplicate(CustomTypeBase* pRetVal) const override;

	void deserialize(const nByte* pData, short length) override;

	short serialize(nByte* pRetVal) const override;

	ExitGames::Common::JString& toString(ExitGames::Common::JString& retStr, bool withTypes) const override;

private:
	DesyncPacket desyncPacket_{};
};
}

#endif //SPLASHONLINE_NETWORK_PACKET_H_
