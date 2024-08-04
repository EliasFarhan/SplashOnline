//
// Created by unite on 02.08.2024.
//

#ifndef SPLASHONLINE_PLAYER_SOUND_H_
#define SPLASHONLINE_PLAYER_SOUND_H_

#include "game/const.h"
#include <string_view>


namespace splash
{

enum class PlayerSoundId
{
	JETPACKON,
	JETPACK,
	JETPACKOFF,

	LAND,
	DASHLAND,
	WALK,

	GUN,
	IMPACT1,
	IMPACT2,
	IMPACT3,
	IMPACT4,

	EJECT,

	COLLISION,

	STOMPPREP,
	STOMPING,
	STOMPIMPACT,
	LENGTH
};

std::string_view GetPlayerSoundEvent(PlayerSoundId soundId);
std::string_view GetPlayerDeathSoundEvent(Character character);



}
#endif //SPLASHONLINE_PLAYER_SOUND_H_
