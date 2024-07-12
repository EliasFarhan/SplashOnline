#include "engine/engine.h"

#include <SDL.h>

namespace splash
{
Engine* instance = nullptr;

void Engine::Run()
{
	instance = this;
	//TODO init three jobsystem queues
	otherQueue_ = jobSystem_.SetupNewQueue(1);
	networkQueue_ = jobSystem_.SetupNewQueue(1);

	Begin();
	bool isOpen = true;
	while (isOpen)
	{
		SDL_Event e;
//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
//User requests quit
			if (e.type == SDL_QUIT)
			{
				isOpen = false;
			}
			//TODO do something for event
		}

		graphicsManager_.Update(0.0f);


	}
	End();

}

void Engine::Begin()
{
	jobSystem_.Begin();
	graphicsManager_.Begin();
}

void Engine::End()
{
	graphicsManager_.End();
	jobSystem_.End();
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