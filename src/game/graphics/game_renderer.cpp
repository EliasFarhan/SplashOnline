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
}
void GameRenderer::Update(float dt)
{
	playerRenderer_.Update(dt);
	bulletRenderer_.Update(dt);
}
void GameRenderer::End()
{
	RemoveDrawInterface(this);
	bulletRenderer_.End();
	playerRenderer_.End();
}
}