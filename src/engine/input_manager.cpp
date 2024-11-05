//
// Created by efarhan on 26/07/24.
//

#include "engine/input_manager.h"
#include "graphics/graphics_manager.h"
#include "utils/log.h"
#include "game/game_manager.h"

#include <fmt/format.h>

#include "engine/engine.h"
#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace splash
{
InputManager::InputManager(std::string_view inputFile) : inputFile_(inputFile.data())
{
}

static int callback(void *data, int argc, char **argv, char **azColName)
{
	std::vector<PlayerInput>& inputs = *static_cast<std::vector<PlayerInput>*>(data);

	PlayerInput currentInput{};
	std::ptrdiff_t frame = -1;
	for(int i = 0; i < argc; i++)
	{
		std::string_view arg = azColName[i];
		if("frame" == arg)
		{
			frame = std::stoi(argv[i]);
		}
		else if(arg == "move_x")
		{
			currentInput.moveDirX = neko::Fixed8::fromUnderlyingValue(std::stoi(argv[i]));
		}
		else if(arg == "move_y")
		{
			currentInput.moveDirY = neko::Fixed8::fromUnderlyingValue(std::stoi(argv[i]));
		}
		else if(arg == "target_x")
		{
			currentInput.targetDirX = neko::Fixed8::fromUnderlyingValue(std::stoi(argv[i]));
		}
		else if(arg == "target_y")
		{
			currentInput.targetDirY = neko::Fixed8::fromUnderlyingValue(std::stoi(argv[i]));
		}
		else if(arg == "button")
		{
			currentInput.buttons = std::stoi(argv[i]);
		}
	}
	if(frame > std::ssize(inputs))
	{
		inputs.resize(frame);
	}
	inputs[frame] = currentInput;
	return 0;
}

void InputManager::Begin()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	if(inputFile_.empty())
	{
		AddEventListener(this);
		SDL_GameControllerAddMappingsFromFile("data/config/gamecontrollerdb.txt");
		controller_ = FindGameController();
		if(controller_ == nullptr)
		{
			LogWarning("No controller attached");
		}
	}
	else
	{
		playerInputs_.resize(3000);
		//TODO load sqlite data from local input file db
		loadInputsJob_ = std::make_unique<neko::FuncJob>([this]()
		{
#ifdef TRACY_ENABLE
			ZoneNamed(loadInputDb, "Load Input Db");
#endif
			auto result = sqlite3_open(inputFile_.data(), &db_);
			if(result != SQLITE_OK)
			{
				LogError(fmt::format("Could not open db: {}", inputFile_.data()));
				return;
			}
			constexpr auto inputQuery = "SELECT frame, move_x, move_y, target_x, target_y, button FROM local_inputs;";
			char* errorMsg = nullptr;
			result = sqlite3_exec(db_, inputQuery, callback, &playerInputs_, &errorMsg);
			if(result != SQLITE_OK)
			{
				LogError(fmt::format("Could not load player inputs (error: {}) from db: {}", result, inputFile_.data()));
			}

			sqlite3_close(db_);
		});
		ScheduleAsyncJob(loadInputsJob_.get());
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
	default:
		break;
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
	if(!inputFile_.empty())
	{
		return playerInputs_[GetCurrentFrame()];
	}
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