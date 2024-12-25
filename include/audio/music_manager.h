//
// Created by unite on 19.07.2024.
//

#ifndef SPLASHONLINE_MUSIC_MANAGER_H
#define SPLASHONLINE_MUSIC_MANAGER_H

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#endif
#include <fmod_studio.hpp>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include <string_view>

namespace splash
{
namespace MusicManager
{
	void Begin();
	void End();
};

void PlayMusic();
void SetMusicParameter(std::string_view name, float value);
float GetMusicVolume();
void SetMusicVolume(float newVolume);
}

#endif //SPLASHONLINE_MUSIC_MANAGER_H
