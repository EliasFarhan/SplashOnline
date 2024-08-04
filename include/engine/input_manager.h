//
// Created by efarhan on 26/07/24.
//

#ifndef SPLASHONLINE_INPUT_MANAGER_H
#define SPLASHONLINE_INPUT_MANAGER_H

#include <SDL_gamecontroller.h>
#include <SDL_events.h>

#include "math/fixed.h"
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
	neko::Fixed8 moveDirX{};
	neko::Fixed8 moveDirY{};
	neko::Fixed8 targetDirX{};
	neko::Fixed8 targetDirY{};
	std::uint8_t buttons{};

	[[nodiscard]] constexpr bool GetStomp() const { return (buttons & STOMP) == STOMP; }
	[[nodiscard]] constexpr bool GetConfirm() const { return (buttons & CONFIRM) == CONFIRM; }
	[[nodiscard]] constexpr bool GetCancel() const { return (buttons & CANCEL) == CANCEL; }
	constexpr void SetStomp(bool stomp) { stomp ? buttons |= STOMP : buttons &= ~STOMP; }
	constexpr void SetConfirm(bool confirm) { confirm ? buttons |= CONFIRM : buttons &= ~CONFIRM; }
	constexpr void SetCancel(bool cancel) { cancel ? buttons |= CANCEL : buttons &= ~CANCEL; }
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
	static constexpr float deadZone = 0.1f;
};
}

#endif //SPLASHONLINE_INPUT_MANAGER_H
