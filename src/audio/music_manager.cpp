//
// Created by unite on 19.07.2024.
//

#include "audio/music_manager.h"
#include "audio/audio_manager.h"

#include <exception>

namespace splash
{

static MusicManager* instance = nullptr;

void MusicManager::Begin()
{
	instance = this;
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
	instance = nullptr;
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
float MusicManager::GetVolume() const
{
	if(eventInstance_ != nullptr)
	{
		float volume;
		eventInstance_->getVolume(&volume);
		return volume;
	}
	return 0;
}
void MusicManager::SetVolume(float newVolume)
{
	if(eventInstance_ != nullptr)
	{
		eventInstance_->setVolume(newVolume);
	}
}

void PlayMusic()
{
	if(instance == nullptr)
	{
		return;
	}
	instance->Play();
}

void SetMusicParameter(std::string_view name, float value)
{
	if(instance == nullptr)
	{
		return;
	}
	instance->SetParameter(name, value);
}

float GetMusicVolume()
{
	if(instance == nullptr)
	{
		return 0.0f;
	}
	return instance->GetVolume();
}

void SetMusicVolume(float newVolume)
{
	if(instance == nullptr)
	{
		return;
	}
	instance->SetVolume(newVolume);
}
}