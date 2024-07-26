//
// Created by efarhan on 26/07/24.
//

#include "engine/input_manager.h"
#include "utils/log.h"


namespace splash
{
void InputManager::Begin()
{
	controller_ = FindGameController();
	if(controller_ == nullptr)
	{
		LogWarning("No controller attached");
	}
}

SDL_GameController* InputManager::FindGameController()
{
	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		if (SDL_IsGameController(i)) {
			return SDL_GameControllerOpen(i);
		}
	}

	return nullptr;
}

void InputManager::ManageEvent(const SDL_Event& event)
{
	switch (event.type)
	{
	case SDL_CONTROLLERDEVICEADDED:
	{
		if(controller_ == nullptr)
		{
			controller_ = SDL_GameControllerOpen(event.cdevice.which);
		}
		break;
	}
	case SDL_CONTROLLERDEVICEREMOVED:
	{
		if(controller_ != nullptr && event.cdevice.which == GetControllerInstanceId(controller_))
		{
			SDL_GameControllerClose(controller_);
			controller_ = FindGameController();
			if(controller_ == nullptr)
			{
				LogWarning("Controller disconnected and no alternative");
			}
		}
		break;
	}
	}
}

SDL_JoystickID InputManager::GetControllerInstanceId(SDL_GameController* controller)
{
	return SDL_JoystickInstanceID(
			SDL_GameControllerGetJoystick(controller));

}

void InputManager::End()
{
	if(controller_ != nullptr)
	{
		SDL_GameControllerClose(controller_);
		controller_ = nullptr;
	}
}
}