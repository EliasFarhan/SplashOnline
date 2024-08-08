//
// Created by unite on 06.07.2024.
//

#include "game/player_character.h"
#include "game/game_systems.h"
#include "game/const.h"

namespace splash
{

static constexpr std::array<neko::Vec2f, MaxPlayerNmb> spawnPositions
	{{
		 {neko::Fixed16{-4.77f}, neko::Fixed16{-1.6f}},
		 {neko::Fixed16{4.13f}, neko::Fixed16{-1.79f}},
		 {neko::Fixed16{-1.65f}, neko::Fixed16{0.86f}},
		 {neko::Fixed16{1.38f}, neko::Fixed16{0.53f}},
	}};

PlayerManager::PlayerManager(GameSystems* gameSystems): gameSystems_(gameSystems)
{

}
void PlayerManager::Begin()
{

}
void PlayerManager::Tick()
{

}
void PlayerManager::End()
{

}
}