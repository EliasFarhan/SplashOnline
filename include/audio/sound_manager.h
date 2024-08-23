//
// Created by unite on 19.07.2024.
//

#ifndef SPLASHONLINE_SOUND_MANAGER_H
#define SPLASHONLINE_SOUND_MANAGER_H

#include <string_view>
namespace splash
{

enum class GameSoundId
{
	CONFIRM,
	CANCEL,
	BLAST,
	SUDDENDEATH,
	VOICE5,
	VOICE4,
	VOICE3,
	VOICE2,
	VOICE1,
	ENDGAME,
	VOICE30,
	LENGTH
};

std::string_view GetGameSoundEvent(GameSoundId soundId);

}
#endif //SPLASHONLINE_SOUND_MANAGER_H
