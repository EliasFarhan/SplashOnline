//
// Created by unite on 19.07.2024.
//

#ifndef SPLASHONLINE_MUSIC_MANAGER_H
#define SPLASHONLINE_MUSIC_MANAGER_H

#include <fmod_studio.hpp>

namespace splash
{
class MusicManager
{
public:
	void Begin();
	void End();
private:
	FMOD::Studio::EventInstance* eventInstance_ = nullptr;
};
}

#endif //SPLASHONLINE_MUSIC_MANAGER_H
