//
// Created by unite on 26.08.2024.
//

#include "game/graphics/end_screen_view.h"
#include "rollback/rollback_manager.h"
#include "graphics/texture_manager.h"
#include "graphics/graphics_manager.h"
#include "graphics/const.h"

namespace splash
{

void EndScreenView::SetPlayerScore(const std::array<int, MaxPlayerNmb>& playerScores)
{
	playerScores_ = playerScores;
	auto sortedPlayerScores = playerScores;
	std::sort(sortedPlayerScores.begin(), sortedPlayerScores.end(), std::greater<>());
	for(int i = 0; i < MaxPlayerNmb; i++)
	{
		playerRanks_[i] = (int)std::distance(sortedPlayerScores.begin(), std::find(sortedPlayerScores.begin(), sortedPlayerScores.end(), playerScores[i]))+1;
	}
}

void EndScreenView::Update(float dt)
{

}

void EndScreenView::Draw()
{
	auto* renderer = GetRenderer();
	for(int i = 0; i < MaxPlayerNmb; i++)
	{
		if(!IsValid(i))
		{
			continue;
		}
		const auto textureId = playerRanks_[i] == 1 ?
							   (TextureManager::TextureId)((int)TextureManager::TextureId::WIN_P1_CAT+i) :
							   (TextureManager::TextureId)((int)TextureManager::TextureId::LOSE_P1_CAT+i);
		auto* texture = GetTexture(textureId);
		auto textureSize = (neko::Vec2<float>)GetTextureSize(textureId)/pixelPerMeter;

		//todo position player head
		const auto rect = GetDrawingRect({}, neko::Vec2f(textureSize));
		SDL_RenderCopy(renderer, texture, nullptr, &rect);
	}
}
}