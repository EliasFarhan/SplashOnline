//
// Created by unite on 12.07.2024.
//

#ifndef SPLASHONLINE_ENGINE_H_
#define SPLASHONLINE_ENGINE_H_

#include "graphics/graphics_manager.h"
#include "engine/window.h"
#include "engine/input_manager.h"
#include "engine/system.h"
#include "audio/audio_manager.h"
#include <thread/job_system.h>


namespace splash
{

class Engine
{
public:
	Engine();

	explicit Engine(std::string_view inputFile);
	void Run();

	void ScheduleJob(neko::Job* job);
	void AddSystem(SystemInterface* system);
	void RemoveSystem(SystemInterface* system);
	PlayerInput GetPlayerInput() const;
	void ScheduleNetJob(neko::Job* pJob);
	float GetDeltaTime() const {return dt_;}
	Uint64 GetCurrentFrameTime() const{return current_;}
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
	float dt_ = 0.0f;
	Uint64 current_ = 0u;
};

void ScheduleAsyncJob(neko::Job* job);
void ScheduleNetJob(neko::Job* job);
void AddSystem(SystemInterface* system);
void RemoveSystem(SystemInterface* system);
PlayerInput GetPlayerInput();
float GetDeltaTime();
Uint64 GetCurrentFrameTime();
}

#endif //SPLASHONLINE_ENGINE_H_
