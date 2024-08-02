//
// Created by unite on 02.08.2024.
//

#ifndef SPLASHONLINE_GAME_CONST_H_
#define SPLASHONLINE_GAME_CONST_H_

#include "math/fixed.h"
namespace splash
{
static constexpr neko::Fixed16 fixedDeltaTime{1, 60};

enum class Character
{
	CAT,
	OWL,
	LUCHA,
	ROBO,
	LENGTH
};

static constexpr int MaxPlayerNmb = (int)Character::LENGTH;
}

#endif //SPLASHONLINE_GAME_CONST_H_
