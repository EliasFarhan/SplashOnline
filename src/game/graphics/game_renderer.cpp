//
// Created by unite on 02.08.2024.
//
#include "game/graphics/game_renderer.h"

namespace splash
{
static GameRenderer* instance = nullptr;
void GameRenderer::Draw()
{
	levelRenderer_.Draw();
	playerRenderer_.Draw();
	bulletRenderer_.Draw();
}
void GameRenderer::SetGraphicsIndex(int index)
{
	graphicIndex = index;
}
int GameRenderer::GetGraphicsIndex() const
{
	return graphicIndex;
}
void GameRenderer::Begin()
{
	AddDrawInterface(this);
	playerRenderer_.Begin();
	bulletRenderer_.Begin();
	levelRenderer_.Begin();
}
void GameRenderer::Update(float dt)
{
	timeSinceTick_ += dt;
	levelRenderer_.Update();
	playerRenderer_.Update(dt);
	bulletRenderer_.Update(dt);
}
void GameRenderer::End()
{
	RemoveDrawInterface(this);
	levelRenderer_.End();
	bulletRenderer_.End();
	playerRenderer_.End();
}

GameRenderer::GameRenderer(const GameSystems* gameSystems):
	playerRenderer_(gameSystems),
	bulletRenderer_(gameSystems)
{
	instance = this;
}

void GameRenderer::Tick()
{
	timeSinceTick_ -= (float)fixedDeltaTime;
}

float GameRenderer::GetTimeSinceTick() const
{
	return timeSinceTick_;
}
}