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
			playerRanks_[i] = static_cast<int>(std::distance(sortedPlayerScores.begin(), std::find(sortedPlayerScores.begin(), sortedPlayerScores.end(), playerScores[i])))+1;
			const auto textureId = playerRanks_[i] == 1 ?
								   static_cast<TextureManager::TextureId>(static_cast<int>(TextureManager::TextureId::WIN_P1_CAT)+i) :
								   static_cast<TextureManager::TextureId>(static_cast<int>(TextureManager::TextureId::LOSE_P1_CAT)+i);
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
							   static_cast<TextureManager::TextureId>(static_cast<int>(TextureManager::TextureId::WIN_P1_CAT)+i) :
							   static_cast<TextureManager::TextureId>(static_cast<int>(TextureManager::TextureId::LOSE_P1_CAT)+i);
		const auto textureSize = static_cast<neko::Vec2<float>>(GetTextureSize(textureId))/pixelPerMeter;
		const neko::Vec2f size = neko::Vec2f(neko::Vec2<float>(gameWindowSize))/neko::Scalar {pixelPerMeter};
		neko::Scalar x = neko::Scalar{static_cast<float>(i+1)}*size.x/neko::Scalar(sixit::guidelines::precision_cast<float>(playerCount_+1))-size.x/neko::Scalar{2};
		neko::Scalar y = neko::Scalar{static_cast<float>((playerCount_+1)-playerRanks_[i])}*size.y/neko::Scalar(sixit::guidelines::precision_cast<float>(playerCount_+1))-size.y/neko::Scalar{2};
		y = neko::Lerp(neko::Scalar{}, y, static_cast<neko::Scalar>(transitionTimer_.CurrentRatio()));

		const auto rect = GetDrawingRect({x, y}, neko::Vec2f(textureSize));
		SDL_RenderCopy(renderer, playerHeads_[i], nullptr, &rect);
	}
}
}