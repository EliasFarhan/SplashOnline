//
// Created by unite on 02.08.2024.
//
#include "game/game_systems.h"
#include "utils/log.h"

#include <fmt/format.h>
#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace splash
{

GameSystems::GameSystems(): playerManager_(this), bulletManager_(this), level_(&physicsManager_),
							physicsManager_({{}, neko::Scalar{-9.81f}})
{
	physicsManager_.SetBSH(&quadTree_);
	physicsManager_.SetContactListener(this);
}

void GameSystems::Begin()
{
	playerManager_.Begin();
	level_.Begin();
	bulletManager_.Begin();

}

void GameSystems::Tick()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	physicsManager_.Step(fixedDeltaTime);
	playerManager_.Tick();
	bulletManager_.Tick();
}

void GameSystems::End()
{
	bulletManager_.End();
	playerManager_.End();
	level_.End();
}
void GameSystems::OnTriggerEnter(const neko::ColliderPair& p)
{
	const auto& c1 = physicsManager_.collider(p.c1);
	const auto* userData1 = static_cast<const ColliderUserData*>(c1.userData);
	const auto& c2 = physicsManager_.collider(p.c2);
	const auto* userData2 = static_cast<const ColliderUserData*>(c2.userData);

	//LogDebug(fmt::format("On Trigger Enter: c{} c{}", p.c1.index, p.c2.index));
	if(userData1->type == ColliderType::PLAYER)
	{
		playerManager_.OnTriggerEnter(p.c1, userData1->playerNumber, c2);
	}
	if(userData2->type == ColliderType::PLAYER)
	{
		playerManager_.OnTriggerEnter(p.c2, userData2->playerNumber, c1);
	}
	if(userData1->type == ColliderType::BULLET)
	{
		bulletManager_.OnTriggerEnter(p.c1, c2);
	}
	if(userData2->type == ColliderType::BULLET)
	{
		bulletManager_.OnTriggerEnter(p.c2, c1);
	}

}
void GameSystems::OnTriggerExit(const neko::ColliderPair& p)
{
	const auto& c1 = physicsManager_.collider(p.c1);
	const auto* userData1 = static_cast<const ColliderUserData*>(c1.userData);
	const auto& c2 = physicsManager_.collider(p.c2);
	const auto* userData2 = static_cast<const ColliderUserData*>(c2.userData);

	//LogDebug(fmt::format("On Trigger Exit: c{} c{}", p.c1.index, p.c2.index));
	if(userData1->type == ColliderType::PLAYER)
	{
		playerManager_.OnTriggerExit(p.c1, userData1->playerNumber, c2);
	}
	if(userData2->type == ColliderType::PLAYER)
	{
		playerManager_.OnTriggerExit(p.c2, userData2->playerNumber, c1);
	}
}
void GameSystems::OnCollisionEnter([[maybe_unused]] const neko::ColliderPair& p)
{
	//LogDebug(fmt::format("On Collision Enter: c{} c{}", p.c1.index, p.c2.index));
}
void GameSystems::OnCollisionExit([[maybe_unused]] const neko::ColliderPair& p)
{
	//LogDebug(fmt::format("On Collision Exit: c{} c{}", p.c1.index, p.c2.index));
}
void GameSystems::SetPlayerInput(neko::Span<PlayerInput> playerInputs)
{
	playerManager_.SetPlayerInput(playerInputs);
}

void GameSystems::SetPreviousPlayerInput(neko::Span<PlayerInput> playerInputs)
{
	playerManager_.SetPreviousPlayerInput(playerInputs);
}

Checksum<(int)BulletChecksumIndex::LENGTH+(int)PlayerChecksumIndex::LENGTH> GameSystems::CalculateChecksum() const
{
	Checksum<(int)BulletChecksumIndex::LENGTH+(int)PlayerChecksumIndex::LENGTH> gameSystemsChecksum{};
	gameSystemsChecksum.push_back(playerManager_.CalculateChecksum());
	gameSystemsChecksum.push_back(bulletManager_.CalculateChecksum());
	return gameSystemsChecksum;
}

void GameSystems::RollbackFrom(const GameSystems& system)
{
	playerManager_.RollbackFrom(system.playerManager_);
	bulletManager_.RollbackFrom(system.bulletManager_);
	physicsManager_.CopyFrom(system.physicsManager_);

}


}