//
// Created by efarhan on 26/07/24.
//

#ifndef SPLASHONLINE_INPUT_MANAGER_H
#define SPLASHONLINE_INPUT_MANAGER_H

#include <SDL_gamecontroller.h>
#include <SDL_events.h>

#include "maths/fixed.h"
#include <cstdint>

namespace splash
{

struct PlayerInput
{
	enum Button
	{
		STOMP = 1u,
		CONFIRM = 1u << 1u,
		CANCEL = 1u << 2u
	};
	Fixed8 moveDirX{};
	Fixed8 moveDirY{};
	Fixed8 targetDirX{};
	Fixed8 targetDirY{};
	std::uint8_t buttons{};
};

class InputManager
{
public:
	void Begin();
	void ManageEvent(const SDL_Event& event);
	void End();

	[[nodiscard]] PlayerInput GetPlayerInput() const;

private:
	static SDL_GameController* FindGameController();
	static SDL_JoystickID GetControllerInstanceId(SDL_GameController *controller);
	SDL_GameController* controller_ = nullptr;
};
}

#endif //SPLASHONLINE_INPUT_MANAGER_H
