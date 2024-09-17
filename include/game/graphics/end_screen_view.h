//
// Created by unite on 26.08.2024.
//

#ifndef SPLASHONLINE_END_SCREEN_VIEW_H_
#define SPLASHONLINE_END_SCREEN_VIEW_H_

#include "game/const.h"
#include "utils/timer.h"
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
	std::array<SDL_Texture*, MaxPlayerNmb> playerHeads_{};
	Timer<float, 1.0f> transitionTimer_{-1.0f};
	int playerCount_ = 0;
};

}

#endif //SPLASHONLINE_END_SCREEN_VIEW_H_
