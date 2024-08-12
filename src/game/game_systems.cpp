//
// Created by unite on 02.08.2024.
//
#include "game/game_systems.h"
#include "utils/log.h"

#include <fmt/format.h>

namespace splash
{

GameSystems::GameSystems(): playerManager_(this), bulletManager_(this), physicsManager_({{}, neko::Scalar{-9.81f}}),
							level_(&physicsManager_)
{
	physicsManager_.SetBSH(&quadTree_);
	physicsManager_.SetContactListener(this);
}

void GameSystems::Begin()
{
	playerManager_.Begin();
	level_.Begin();

}

void GameSystems::Tick()
{
	physicsManager_.Step(fixedDeltaTime);
	playerManager_.Tick();
}

void GameSystems::End()
{
	playerManager_.End();
	level_.End();
}
void GameSystems::OnTriggerEnter(const neko::ColliderPair& p)
{
	LogDebug(fmt::format("On Trigger Enter: c{} c{}", p.c1.index, p.c2.index));
}
void GameSystems::OnTriggerExit(const neko::ColliderPair& p)
{
	LogDebug(fmt::format("On Trigger Exit: c{} c{}", p.c1.index, p.c2.index));
}
void GameSystems::OnCollisionEnter(const neko::ColliderPair& p)
{
	LogDebug(fmt::format("On Collision Enter: c{} c{}", p.c1.index, p.c2.index));
}
void GameSystems::OnCollisionExit(const neko::ColliderPair& p)
{
	LogDebug(fmt::format("On Collision Exit: c{} c{}", p.c1.index, p.c2.index));
}

}