//
// Created by efarhan on 26/07/24.
//

#include "engine/input_manager.h"
#include "graphics/graphics_manager.h"
#include "utils/log.h"
#include "game/game_manager.h"
#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace splash
{
void InputManager::Begin()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	AddEventListener(this);
	SDL_GameControllerAddMappingsFromFile("data/config/gamecontrollerdb.txt");
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
	const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
	if( currentKeyStates[ SDL_SCANCODE_UP ] || currentKeyStates[SDL_SCANCODE_W] )
	{
		input.moveDirY = neko::Fixed8{1.0f};
	}
	if( currentKeyStates[ SDL_SCANCODE_DOWN ] || currentKeyStates[SDL_SCANCODE_S] )
	{
		input.moveDirY = neko::Fixed8{-1.0f};
	}
	if( currentKeyStates[ SDL_SCANCODE_LEFT ]  || currentKeyStates[SDL_SCANCODE_A])
	{
		input.moveDirX = neko::Fixed8{-1.0f};
	}
	if( currentKeyStates[ SDL_SCANCODE_RIGHT ]  || currentKeyStates[SDL_SCANCODE_D])
	{
		input.moveDirX = neko::Fixed8{1.0f};
	}

	if(currentKeyStates[SDL_SCANCODE_LSHIFT])
	{
		input.SetStomp(true);
	}
	neko::Vec2i mousePos;
	const auto mouseButtonState = SDL_GetMouseState(&mousePos.x, &mousePos.y);
	mousePos -= GetOffset();
	auto targetDir = neko::Vec2<float>(mousePos-GetPlayerScreenPos());
	if(targetDir.Length() > 1.0f)
	{
		targetDir = targetDir.Normalized();
	}
	if(mouseButtonState & SDL_BUTTON(1))
	{
		input.targetDirX = neko::Fixed8{targetDir.x};
		input.targetDirY = neko::Fixed8{-targetDir.y};
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