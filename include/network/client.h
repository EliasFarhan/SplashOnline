//
// Created by unite on 04.08.2024.
//

#ifndef SPLASHONLINE_NETWORK_CLIENT_H_
#define SPLASHONLINE_NETWORK_CLIENT_H_

#include "engine/system.h"
#include "graphics/gui_renderer.h"
#include "network/packet.h"
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wdtor-name"
#endif
#include <neko/network_manager.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#include <container/span.h>
#include <thread/job_system.h>

namespace splash
{


namespace NetworkClient
{
enum class State
{
	UNCONNECTED,
	CONNECTING,
	CHOOSING_REGIONS,
	CONNECTED_TO_MASTER,
	JOINING,
	IN_ROOM,
	IN_GAME,
};
State GetState();
bool IsValid();

std::array<bool, MaxPlayerNmb> GetConnectedPlayers();
uint8_t GetPlayerIndex();
bool IsMaster();
}

void BeginNetwork(const ExitGames::LoadBalancing::ClientConstructOptions& clientConstructOptions={});
void SendInputPacket(const InputPacket& inputPacket);
void SendConfirmFramePacket(const ConfirmFramePacket& confirmPacket);
neko::Span<InputPacket> GetInputPackets();
neko::Span<ConfirmFramePacket> GetConfirmPackets();
}

#endif //SPLASHONLINE_NETWORK_CLIENT_H_
