//
// Created by efarhan on 26/07/24.
//

#ifndef SPLASHONLINE_INPUT_MANAGER_H
#define SPLASHONLINE_INPUT_MANAGER_H

#include <SDL_gamecontroller.h>
#include <SDL_events.h>

namespace splash
{
class InputManager
{
public:
	void Begin();
	void ManageEvent(const SDL_Event& event);
	void End();

private:
	static SDL_GameController* FindGameController();
	static SDL_JoystickID GetControllerInstanceId(SDL_GameController *controller);
	SDL_GameController* controller_ = nullptr;
};
}

#endif //SPLASHONLINE_INPUT_MANAGER_H
