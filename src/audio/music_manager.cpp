//
// Created by unite on 19.07.2024.
//

#include "audio/music_manager.h"
#include "audio/audio_manager.h"

#include <exception>

namespace splash
{
namespace
{

FMOD::Studio::EventInstance* eventInstance_ = nullptr;
}

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
    if(eventInstance_ != nullptr)
    {
        eventInstance_->stop(FMOD_STUDIO_STOP_IMMEDIATE);
        eventInstance_->release();
    }
}

void PlayMusic()
{
	if(eventInstance_ != nullptr)
	{
		eventInstance_->start();
	}
}

void SetMusicParameter(std::string_view name, float value)
{
	if(eventInstance_ != nullptr)
	{
		eventInstance_->setParameterByName(name.data(), value);
	}
}

float GetMusicVolume()
{
	if(eventInstance_ != nullptr)
	{
		float volume;
		eventInstance_->getVolume(&volume);
		return volume;
	}
	return 0;
}

void SetMusicVolume(float newVolume)
{
	if(eventInstance_ != nullptr)
	{
		eventInstance_->setVolume(newVolume);
	}
}
}