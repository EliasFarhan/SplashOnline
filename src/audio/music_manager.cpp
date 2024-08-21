//
// Created by unite on 19.07.2024.
//

#include "audio/music_manager.h"
#include "audio/audio_manager.h"

#include <exception>

namespace splash
{

void MusicManager::Begin()
{
	FMOD::Studio::EventDescription* eventDescription = GetEventDescription("event:/Music/Music"); //"event:/Music/Music"

	if(eventDescription->createInstance(&eventInstance_) != FMOD_OK)
	{
		std::terminate();
	}

}
void MusicManager::End()
{
	eventInstance_->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	eventInstance_->release();
}

void MusicManager::Play()
{
	if(eventInstance_ != nullptr)
	{
		eventInstance_->start();
	}
}

void MusicManager::SetParameter(std::string_view name, float value)
{
	if(eventInstance_ != nullptr)
	{
		eventInstance_->setParameterValue(name.data(), value);
	}
}
}