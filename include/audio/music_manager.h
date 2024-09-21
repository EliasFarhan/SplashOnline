//
// Created by unite on 19.07.2024.
//

#ifndef SPLASHONLINE_MUSIC_MANAGER_H
#define SPLASHONLINE_MUSIC_MANAGER_H

#include <fmod_studio.hpp>

#include <string_view>

namespace splash
{
class MusicManager
{
public:
	void Begin();
	void End();
	void Play();
	void SetParameter(std::string_view name, float value);
	[[nodiscard]] float GetVolume() const;
	void SetVolume(float newVolume);
private:
	FMOD::Studio::EventInstance* eventInstance_ = nullptr;
};

void PlayMusic();
void SetMusicParameter(std::string_view name, float value);
float GetMusicVolume();
void SetMusicVolume(float newVolume);
}

#endif //SPLASHONLINE_MUSIC_MANAGER_H
