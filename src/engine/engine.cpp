#include "engine/engine.h"
#include "utils/log.h"

#include <fmt/format.h>
#include <SDL.h>
#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif
namespace splash
{
Engine* instance = nullptr;

void Engine::Run()
{
	instance = this;
	otherQueue_ = jobSystem_.SetupNewQueue(1);
	networkQueue_ = jobSystem_.SetupNewQueue(1);

	Begin();
	while (window_.IsOpen())
	{
		float dt = 0.0f;
		window_.Update();

		graphicsManager_.Update(dt);
		for(auto* system : systems_)
		{
			system->Update(dt);
		}

		graphicsManager_.PreDraw();
		graphicsManager_.Draw();
		graphicsManager_.PostDraw();
#ifdef TRACY_ENABLE
		FrameMark;
#endif
	}
	End();

}

void Engine::Begin()
{
	jobSystem_.Begin();
	/*
	* Initialises the SDL video subsystem (as well as the events subsystem).
	* Returns 0 on success or a negative error code on failure using SDL_GetError().
	*/
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0)
	{
		LogError(fmt::format("SDL failed to initialise: {}", SDL_GetError()));
		return;
	}
	window_.Begin();
	inputManager_.Begin();
	graphicsManager_.Begin();
}

void Engine::End()
{
	graphicsManager_.End();
	inputManager_.End();
	window_.End();
	jobSystem_.End();

	/* Shuts down all SDL subsystems */
	SDL_Quit();
}

void Engine::ScheduleJob(neko::Job* job)
{
	jobSystem_.AddJob(job, otherQueue_);
}

void Engine::AddSystem(SystemInterface* system)
{
	auto it = std::find(systems_.begin(), systems_.end(), nullptr);
	if(it != systems_.end())
	{
		*it = system;
		system->SetSystemIndex((int)std::distance(systems_.begin(), it));
	}
	else
	{
		system->SetSystemIndex((int)systems_.size());
		systems_.push_back(system);
	}
}

void Engine::RemoveSystem(SystemInterface* system)
{
	systems_[system->GetSystemIndex()] = nullptr;
}

PlayerInput Engine::GetPlayerInput() const
{
	return inputManager_.GetPlayerInput();
}

Engine::Engine()
{
	instance = this;
}

void AddSystem(SystemInterface* system)
{
	instance->AddSystem(system);
}

void RemoveSystem(SystemInterface* system)
{
	instance->RemoveSystem(system);
}

void ScheduleAsyncJob(neko::Job* job)
{
	instance->ScheduleJob(job);
}

PlayerInput GetPlayerInput()
{
	return instance->GetPlayerInput();
}
}