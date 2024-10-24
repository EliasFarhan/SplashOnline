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

class SpineManager : public spine::TextureLoader
{
public:
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

	SpineManager();
	void Begin();
	void Update(float dt);
	void End();

	void UpdateLoad();

	void load(spine::AtlasPage& page, const spine::String& path) override;

	void unload(void* texture) override;

	bool IsLoaded();

	std::unique_ptr<spine::SkeletonDrawable> CreateSkeletonDrawable(SkeletonId skeletonId);
private:
	std::array<std::unique_ptr<spine::Atlas>, static_cast<int>(AtlasId::LENGTH)> atlases_{};
	std::array<std::unique_ptr<spine::AtlasAttachmentLoader>, static_cast<int>(AtlasId::LENGTH)> attachmentLoaders_;
	std::array<std::unique_ptr<spine::SkeletonJson>, static_cast<int>(SkeletonId::LENGTH)> skeletonJsons_;
	std::array<spine::SkeletonData*, static_cast<int>(SkeletonId::LENGTH)> skeletonData_{};

};
bool IsSpineLoaded();
std::unique_ptr<spine::SkeletonDrawable> CreateSkeletonDrawable(SpineManager::SkeletonId skeletonId);


}
#endif //SPLASHONLINE_SPINE_MANAGER_H_
