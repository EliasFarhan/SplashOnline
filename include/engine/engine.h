//
// Created by unite on 12.07.2024.
//

#ifndef SPLASHONLINE_ENGINE_H_
#define SPLASHONLINE_ENGINE_H_

#include "graphics/graphics_manager.h"
#include "engine/window.h"
#include "engine/input_manager.h"
#include "engine/system.h"
#include <thread/job_system.h>


namespace splash
{



class Engine
{
public:
	Engine();
	void Run();

	void ScheduleJob(neko::Job* job);
	void AddSystem(SystemInterface* system);
	void RemoveSystem(SystemInterface* system);
	PlayerInput GetPlayerInput() const;
private:

	void Begin();
	void End();

	neko::JobSystem jobSystem_{};
	Window window_{};
	GraphicsManager graphicsManager_{};
	InputManager inputManager_;
	std::vector<SystemInterface*> systems_;
	int otherQueue_{};
	int networkQueue_{};
};

void ScheduleAsyncJob(neko::Job* job);
void AddSystem(SystemInterface* system);
void RemoveSystem(SystemInterface* system);
PlayerInput GetPlayerInput();
}

#endif //SPLASHONLINE_ENGINE_H_
