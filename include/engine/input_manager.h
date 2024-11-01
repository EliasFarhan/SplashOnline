#ifndef SPLASHONLINE_INPUT_MANAGER_H
#define SPLASHONLINE_INPUT_MANAGER_H

#include "engine/window.h"

#include <math/fixed.h>

#include <SDL_gamecontroller.h>
#include <SDL_events.h>
#include <sqlite3.h>

#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>
#include <string>

#include "thread/job_system.h"

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

	bool operator==(const PlayerInput& otherInput) const
	{
		return moveDirX == otherInput.moveDirX &&
		moveDirY == otherInput.moveDirY &&
		targetDirX == otherInput.targetDirX &&
		targetDirY == otherInput.targetDirY &&
		buttons == otherInput.buttons;
	}
	bool operator!=(const PlayerInput& otherInput) const
	{
		return !(*this == otherInput);
	}
};

class InputManager : public OnEventInterface
{
public:
	InputManager() = default;
	explicit InputManager(std::string_view inputFile);
	void Begin();
	void ManageEvent(const SDL_Event& event);
	void End();

	[[nodiscard]] PlayerInput GetPlayerInput() const;
	void OnEvent(const SDL_Event& event) override;
	[[nodiscard]] int GetEventListenerIndex() const override;
	void SetEventListenerIndex(int index) override;

	static constexpr float deadZone = 0.2f;
private:
	static SDL_GameController* FindGameController();
	static SDL_JoystickID GetControllerInstanceId(SDL_GameController *controller);
	SDL_GameController* controller_ = nullptr;

	std::unique_ptr<neko::FuncJob> loadInputsJob_;
	std::string inputFile_;
	std::vector<PlayerInput> playerInputs_;
	sqlite3* db_ = nullptr;
	int eventIndex_ = -1;
};
}

#endif //SPLASHONLINE_INPUT_MANAGER_H
