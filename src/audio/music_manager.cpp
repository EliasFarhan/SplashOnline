//
// Created by unite on 19.07.2024.
//

#include "audio/music_manager.h"
#include "audio/audio_manager.h"

namespace splash
{

void MusicManager::Begin()
{
	FMOD::Studio::EventDescription* eventDescription = GetEventDescription("event:/Music/Music"); //"event:/Music/Music"

	if(eventDescription->createInstance(&eventInstance_) != FMOD_OK)
	{
		std::terminate();
	}
	eventInstance_->start();

}
void MusicManager::End()
{
	eventInstance_->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	eventInstance_->release();
}
}