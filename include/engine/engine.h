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

void RunEngine();
void ScheduleAsyncJob(neko::Job* job);
void ScheduleNetJob(neko::Job* job);
void AddSystem(SystemInterface* system);
void RemoveSystem(SystemInterface* system);
float GetDeltaTime();
Uint64 GetCurrentFrameTime();
}

#endif //SPLASHONLINE_ENGINE_H_
