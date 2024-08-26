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
		if(IsValid(i))
		{
			playerRanks_[i] = (int)std::distance(sortedPlayerScores.begin(), std::find(sortedPlayerScores.begin(), sortedPlayerScores.end(), playerScores[i]))+1;
			const auto textureId = playerRanks_[i] == 1 ?
								   (TextureManager::TextureId)((int)TextureManager::TextureId::WIN_P1_CAT+i) :
								   (TextureManager::TextureId)((int)TextureManager::TextureId::LOSE_P1_CAT+i);
			playerHeads_[i] = GetTexture(textureId);
			playerCount_++;
		}
	}
	transitionTimer_.Reset();

}

void EndScreenView::Update(float dt)
{
	transitionTimer_.Update(dt);
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
		const auto textureSize = (neko::Vec2<float>)GetTextureSize(textureId)/pixelPerMeter;
		neko::Vec2f size = neko::Vec2f{gameWindowSize}/neko::Scalar {pixelPerMeter};
		neko::Scalar x = neko::Scalar{i+1}*size.x/neko::Scalar(playerCount_+1)-size.x/neko::Scalar{2};
		neko::Scalar y = neko::Scalar{(playerCount_+1)-playerRanks_[i]}*size.y/neko::Scalar(playerCount_+1)-size.y/neko::Scalar{2};
		y = neko::Lerp(neko::Scalar{}, y, (neko::Scalar )transitionTimer_.CurrentRatio());

		const auto rect = GetDrawingRect({x, y}, neko::Vec2f(textureSize));
		SDL_RenderCopy(renderer, playerHeads_[i], nullptr, &rect);
	}
}
}