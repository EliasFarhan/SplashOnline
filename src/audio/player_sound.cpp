//
// Created by unite on 02.08.2024.
//

#include "audio/player_sound.h"
#include <array>

namespace splash
{

std::string_view GetPlayerSoundEvent(PlayerSoundId soundId)
{
	static constexpr std::array<std::string_view, (int)PlayerSoundId::LENGTH> soundEvents
		{{
			"event:/Jetpack/JetpackON",
			"event:/Jetpack/Jetpack",
			"event:/Jetpack/Jetpack",

			"event:/Land/LandNormal",
			"event:/Land/LandDash",
			"event:/Land/LandWalk",

			"event:/Wata/WataShot",
			"event:/Impacts/Impact1",
			"event:/Impacts/Impact2",
			"event:/Impacts/Impact3",
			"event:/Impacts/Impact4",

			"event:/Death/Eject",

			"event:/Impacts/Collision",

			"event:/Stomps/Stomp_Prepare",
			"event:/Stomps/Stomp_On",
			"event:/Stomps/Stomp_Impact"
		}};
	return soundEvents[(int)soundId];
}
std::string_view GetPlayerDeathSoundEvent(Character character)
{
	static constexpr std::array<std::string_view, (int)Character::LENGTH> soundEvents
		{{
			"event:/Death/DeathCat",
			"event:/Death/DeathOwl",
			"event:/Death/DeathLucha",
			"event:/Death/DeathRobo"
		}};
	return soundEvents[(int)character];
}
}