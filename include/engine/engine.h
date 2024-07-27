//
// Created by unite on 12.07.2024.
//

#ifndef SPLASHONLINE_ENGINE_H_
#define SPLASHONLINE_ENGINE_H_

#include "graphics/graphics_manager.h"
#include <thread/job_system.h>
#include "graphics/graphics_manager.h"
#include "engine/window.h"
#include "engine/input_manager.h"


namespace splash
{

class Engine
{
public:
	void Run();

	void ScheduleJob(neko::Job* job);
private:

	void Begin();
	void End();

	neko::JobSystem jobSystem_{};
	Window window_{};
	GraphicsManager graphicsManager_{};
	InputManager inputManager_;
	int otherQueue_{};
	int networkQueue_{};
};

Engine* GetEngine();

}

#endif //SPLASHONLINE_ENGINE_H_
