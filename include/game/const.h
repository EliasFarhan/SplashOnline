//
// Created by unite on 02.08.2024.
//

#ifndef SPLASHONLINE_GAME_CONST_H_
#define SPLASHONLINE_GAME_CONST_H_

#include <math/vec2.h>
#include <SDL_pixels.h>

#include <array>
#include <string_view>

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

static constexpr auto MaxPlayerNmb = static_cast<int>(Character::LENGTH);

struct Box
{
	neko::Vec2f position{};
	neko::Vec2f offset{};
	neko::Vec2f size{};
};

static constexpr std::array<SDL_Color, MaxPlayerNmb> playerColors
{{
	 {0,149,238, 255},
	 {254,95,0, 255},
	 {238,0,126, 255},
	 {27,255,181, 255},
}};

static constexpr std::array<std::string_view, MaxPlayerNmb> legacyPlayerColorNames
	{{
		"blue",
		"yellow",
		"red",
		"green",
	}};

enum class ColliderType
{
	PLAYER,
	PLATFORM,
	BULLET,
	GAME_LIMIT,
	NONE
};
struct ColliderUserData
{
	ColliderType type = ColliderType::NONE;
	int playerNumber = -1; //Used by player or bullet
};

static constexpr int MaxPlayerInputNmb = 26; //half a second of input


}

#endif //SPLASHONLINE_GAME_CONST_H_
