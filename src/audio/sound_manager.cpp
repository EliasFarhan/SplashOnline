//
// Created by unite on 19.07.2024.
//

#include "audio/sound_manager.h"

#include <array>

namespace splash
{

static constexpr std::array<std::string_view, (int)GameSoundId::LENGTH> soundEventNames
{{
	"event:/GUI/MenuValidation",
	"event:/GUI/MenuCancel",
	"event:/GUI/GameBlast",
	"event:/GUI/GamePanic",
	"event:/InGame/Voice5_Five",
	"event:/InGame/Voice4_Four",
	"event:/InGame/Voice3_Three",
	"event:/InGame/Voice2_Two",
	"event:/InGame/Voice1_One",
	"event:/GUI/GameEnd",
	"event:/InGame/Voice30_Thirty",
}};

std::string_view GetGameSoundEvent(GameSoundId gameSoundId)
{
	return soundEventNames[(int)gameSoundId];
}

}