//
// Created by efarhan on 26/07/24.
//

#include "engine/input_manager.h"
#include "utils/log.h"


namespace splash
{
void InputManager::Begin()
{
	AddEventListener(this);
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

	LogWarning("No controller found");
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
			LogDebug("Controller connected");
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
	RemoveEventListener(this);
	if(controller_ != nullptr)
	{
		SDL_GameControllerClose(controller_);
		controller_ = nullptr;
	}
}
PlayerInput InputManager::GetPlayerInput() const
{
	PlayerInput input{};
	if(controller_ != nullptr)
	{
		const float trigger = static_cast<float>(SDL_GameControllerGetAxis(controller_, SDL_CONTROLLER_AXIS_TRIGGERRIGHT))/32767.0f;
		if(trigger > deadZone)
		{
			input.SetStomp(true);
		}
		input.SetConfirm(SDL_GameControllerGetButton(controller_, SDL_CONTROLLER_BUTTON_A));
		input.SetCancel(SDL_GameControllerGetButton(controller_, SDL_CONTROLLER_BUTTON_B));
		const float leftX = static_cast<float>(SDL_GameControllerGetAxis(controller_, SDL_CONTROLLER_AXIS_LEFTX))/32767.0f;
		const float leftY = static_cast<float>(SDL_GameControllerGetAxis(controller_, SDL_CONTROLLER_AXIS_LEFTY))/32767.0f;
		const float rightX = static_cast<float>(SDL_GameControllerGetAxis(controller_, SDL_CONTROLLER_AXIS_RIGHTX))/32767.0f;
		const float rightY = static_cast<float>(SDL_GameControllerGetAxis(controller_, SDL_CONTROLLER_AXIS_RIGHTY))/32767.0f;
		input.moveDirX = neko::Fixed8{leftX};
		input.moveDirY = -neko::Fixed8{leftY};
		input.targetDirX = neko::Fixed8{rightX};
		input.targetDirY = -neko::Fixed8{rightY};
	}
	return input;
}
void InputManager::OnEvent(const SDL_Event& event)
{
	ManageEvent(event);
}
int InputManager::GetEventListenerIndex() const
{
	return eventIndex_;
}
void InputManager::SetEventListenerIndex(int index)
{
	eventIndex_ = index;
}
}