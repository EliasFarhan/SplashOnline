//
// Created by unite on 02.08.2024.
//

#ifndef SPLASHONLINE_SPINE_MANAGER_H_
#define SPLASHONLINE_SPINE_MANAGER_H_

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wsign-conversion"
#endif
#include <spine/TextureLoader.h>
#include <spine/Atlas.h>
#include <spine-sdl-cpp.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include <memory>
#include <array>

namespace splash
{

namespace SpineManager
{
enum class AtlasId
{
	KWAKWA_LOGO,
	ANNOUNCERS,
	CHARACTERS_NOARM,
	CHARACTERS_ARMS,
	BASEGUN,
	WATA,
	CLOUD,
	FX,
	LENGTH,
};
	enum class SkeletonId
	{
		KWAKWA_LOGO,
		ANNOUNCERS,

		CAT_NOARM,
		LUCHA_NOARM,
		OWL_NOARM,
		ROBOT_NOARM,

		CAT_ARM,
		LUCHA_ARM,
		OWL_ARM,
		ROBOT_ARM,

		BASEGUN,
		WATA,
		CLOUD,

		CLOUD_EJECT,
		CLOUD_EJECT_EXPLOSION,
		DASH,
		DASH_PREP,
		EJECT,
		IMPACT,
		IMPACT2,
		IMPACT_HARD,
		JETBOOM,
		JETPACK1,
		JETPACK2,
		LANDING,
		RELOAD,
		STAR,
		LENGTH

	};

	void Begin();
	void Update(float dt);
	void End();

	void UpdateLoad();


};
bool IsSpineLoaded();
std::unique_ptr<spine::SkeletonDrawable> CreateSkeletonDrawable(SpineManager::SkeletonId skeletonId);


}
#endif //SPLASHONLINE_SPINE_MANAGER_H_
