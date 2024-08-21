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
private:
	FMOD::Studio::EventInstance* eventInstance_ = nullptr;
};
}

#endif //SPLASHONLINE_MUSIC_MANAGER_H
