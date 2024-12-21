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
namespace
{
    std::vector<SystemInterface *> systems_;
    int otherQueue_{};
    int networkQueue_{};
    float dt_ = 0.0f;
    Uint64 current_ = 0u;
}



static void BeginEngine()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	neko::JobSystem::Begin();

	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_INFO);
	/*
	* Initialises the SDL video subsystem (as well as the events subsystem).
	* Returns 0 on success or a negative error code on failure using SDL_GetError().
	*/
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER ) != 0)
	{
		LogError(fmt::format("SDL failed to initialise: {}", SDL_GetError()));
		return;
	}
	BeginWindow();
	BeginInputManager();
	BeginGraphics();
	for(auto* system : systems_)
	{
		if(system == nullptr) continue;
		system->Begin();
	}
}

void EndEngine()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	for(auto* system : systems_)
	{
		if(system == nullptr) continue;
		system->End();
	}
	EndGraphics();
	EndInputManager();
	EndWindow();
	neko::JobSystem::End();

	/* Shuts down all SDL subsystems */
	SDL_Quit();
}

void RunEngine()
{
    otherQueue_ = neko::JobSystem::SetupNewQueue(1);
    networkQueue_ = neko::JobSystem::SetupNewQueue(1);

    BeginEngine();
    auto freq = static_cast<double>(SDL_GetPerformanceFrequency());
    Uint64 previous = SDL_GetPerformanceCounter();
    while (IsWindowOpen())
    {
#ifdef TRACY_ENABLE
        ZoneNamedN(engineLoop, "Engine Loop", true);
#endif
        current_ = SDL_GetPerformanceCounter();
        auto delta =  static_cast<double>(current_ - previous);
        previous = current_;

        dt_ = static_cast<float>(delta/freq);
        UpdateWindow();

        UpdateGraphics();
        for(auto & system : systems_)
        {
            if(system == nullptr) continue;
            system->Update(dt_);
        }

        DrawGraphics();
#ifdef TRACY_ENABLE
        FrameMark;
#endif
    }
    EndEngine();

}





void AddSystem(SystemInterface* system)
{
    auto it = std::find(systems_.begin(), systems_.end(), nullptr);
    if(it != systems_.end())
    {
        *it = system;
        system->SetSystemIndex(static_cast<int>(std::distance(systems_.begin(), it)));
    }
    else
    {
        system->SetSystemIndex(static_cast<int>(systems_.size()));
        systems_.push_back(system);
    }
}

void RemoveSystem(SystemInterface* system)
{
    systems_[system->GetSystemIndex()] = nullptr;
}

void ScheduleAsyncJob(neko::Job* job)
{
#ifdef TRACY_ENABLE
    ZoneScoped;
#endif
    neko::JobSystem::AddJob(job, otherQueue_);
}

void ScheduleNetJob(neko::Job* job)
{
    neko::JobSystem::AddJob(job, networkQueue_);
}

float GetDeltaTime()
{
	return dt_;
}
Uint64 GetCurrentFrameTime()
{
	return current_;
}
}