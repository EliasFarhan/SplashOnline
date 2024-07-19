#include "engine/engine.h"
#include "utils/log.h"

#include <SDL.h>

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
		window_.Update();

		graphicsManager_.Update(0.0f);

		graphicsManager_.PreDraw();
		graphicsManager_.Draw();
		graphicsManager_.PostDraw();
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
		LogError("SDL failed to initialise");
		//fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
		return;
	}
	window_.Begin();
	graphicsManager_.Begin();
}

void Engine::End()
{
	graphicsManager_.End();
	window_.End();
	jobSystem_.End();

	/* Shuts down all SDL subsystems */
	SDL_Quit();
}

void Engine::ScheduleJob(neko::Job* job)
{
	jobSystem_.AddJob(job, otherQueue_);
}

Engine* GetEngine()
{
	return instance;
}
}