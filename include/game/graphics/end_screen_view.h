//
// Created by unite on 26.08.2024.
//

#ifndef SPLASHONLINE_END_SCREEN_VIEW_H_
#define SPLASHONLINE_END_SCREEN_VIEW_H_

#include "game/const.h"
#include "SDL_render.h"

#include <array>

namespace splash
{

class EndScreenView
{
public:
	void Update(float dt);
	void Draw();
	void SetPlayerScore(const std::array<int, MaxPlayerNmb>& playerScores);
private:
	std::array<int, MaxPlayerNmb> playerScores_{};
	std::array<int, MaxPlayerNmb> playerRanks_{};
	std::array<SDL_Texture*, MaxPlayerNmb> playerWinHeads_{};
	std::array<SDL_Texture*, MaxPlayerNmb> playerLoseHeads_{};
};

}

#endif //SPLASHONLINE_END_SCREEN_VIEW_H_
