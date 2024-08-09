//
// Created by unite on 02.08.2024.
//
#include "game/graphics/game_renderer.h"

namespace splash
{

void GameRenderer::Draw()
{
	playerRenderer_.Draw();
	bulletRenderer_.Draw();
	levelRenderer_.Draw();
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
	playerRenderer_.Update(dt);
	bulletRenderer_.Update(dt);
	levelRenderer_.Update();
}
void GameRenderer::End()
{
	RemoveDrawInterface(this);
	levelRenderer_.End();
	bulletRenderer_.End();
	playerRenderer_.End();
}
GameRenderer::GameRenderer(GameSystems* gameSystems)
{

}
}