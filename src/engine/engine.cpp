#include "engine/engine.h"
#include "utils/log.h"

#include <fmt/format.h>
#include <SDL.h>
#include <SDL_timer.h>

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif
namespace splash
{
Engine* instance = nullptr;

void Engine::Run()
{
	otherQueue_ = jobSystem_.SetupNewQueue(1);
	networkQueue_ = jobSystem_.SetupNewQueue(1);

	Begin();
	double freq = (double)SDL_GetPerformanceFrequency();
	Uint64 previous = SDL_GetPerformanceCounter();
	while (window_.IsOpen())
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(engineLoop, "Engine Loop", true);
#endif
		current_ = SDL_GetPerformanceCounter();
		auto delta =  (double)(current_ - previous);
		previous = current_;

		dt_ = (float)(delta/freq);
		window_.Update();

		graphicsManager_.Update(dt_);
		for(std::size_t i = 0; i < systems_.size(); i++)
		{
			if(systems_[i] == nullptr) continue;
			systems_[i]->Update(dt_);
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
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	jobSystem_.Begin();

	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_WARN);
	/*
	* Initialises the SDL video subsystem (as well as the events subsystem).
	* Returns 0 on success or a negative error code on failure using SDL_GetError().
	*/
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER ) != 0)
	{
		LogError(fmt::format("SDL failed to initialise: {}", SDL_GetError()));
		return;
	}
	window_.Begin();
	inputManager_.Begin();
	graphicsManager_.Begin();
	for(auto* system : systems_)
	{
		if(system == nullptr) continue;
		system->Begin();
	}
}

void Engine::End()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	for(auto* system : systems_)
	{
		if(system == nullptr) continue;
		system->End();
	}
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
	systems_.reserve(15);
}
void Engine::ScheduleNetJob(neko::Job* pJob)
{
	jobSystem_.AddJob(pJob, networkQueue_);
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
void ScheduleNetJob(neko::Job* job)
{
	instance->ScheduleNetJob(job);
}

float GetDeltaTime()
{
	return instance->GetDeltaTime();
}
Uint64 GetCurrentFrameTime()
{
	return instance->GetCurrentFrameTime();
}
}