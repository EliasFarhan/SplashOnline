
#include "engine/engine.h"
#include "utils/log.h"

#include <SDL_main.h>
#include <spine-sdl-cpp.h>
#include <fmt/format.h>

#include <memory>

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>
#endif

namespace splash
{
class SpineTest : public SystemInterface, public DrawInterface
{
public:
	SpineTest()
	{
		AddSystem(this);
		AddDrawInterface(this);
	}
	void Begin() override
	{
#ifdef TRACY_ENABLE
		ZoneScoped;
#endif
		auto* renderer = GetRenderer();
#ifdef TRACY_ENABLE
		TracyCZoneN(textureLoader, "Create Texture Loader", true);
#endif
		textureLoader_ = std::make_unique<spine::SDLTextureLoader>(renderer);
#ifdef TRACY_ENABLE
		TracyCZoneEnd(textureLoader);
		TracyCZoneN(atlasCreate, "Create Atlas", true);
#endif
		atlas_ = std::make_unique<spine::Atlas>("data/spine/chars/SBPChars_ver4_2.atlas", textureLoader_.get());
#ifdef TRACY_ENABLE
		TracyCZoneEnd(atlasCreate);
		TracyCZoneN(attachmentLoader, "Create Atlas Attachment Loader", true);
#endif
		attachmentLoader_ = std::make_unique<spine::AtlasAttachmentLoader>(atlas_.get());
#ifdef TRACY_ENABLE
		TracyCZoneEnd(attachmentLoader);
		TracyCZoneN(skeletonJson, "Create Skeleton Json", true);
#endif
		skeletonJson_ = std::make_unique<spine::SkeletonJson>(attachmentLoader_.get());
#ifdef TRACY_ENABLE
		TracyCZoneEnd(skeletonJson);
		TracyCZoneN(skeletonData, "Create Skeleton Data", true);
#endif
		skeletonData_ = skeletonJson_->readSkeletonDataFile("data/spine/chars/cat_noarm.skel");
#ifdef TRACY_ENABLE
		TracyCZoneEnd(skeletonData);
		TracyCZoneN(skeletonDrawable, "Create Skeleton Drawable", true);
#endif
		skeletonDrawable_ = std::make_unique<spine::SkeletonDrawable>(skeletonData_);
#ifdef TRACY_ENABLE
		TracyCZoneEnd(skeletonDrawable);
#endif
		skeletonDrawable_->animationState->setAnimation(0, "idle", true);
		auto& anims = skeletonData_->getAnimations();
		for(std::size_t i = 0; i < anims.size(); i++)
		{
			LogDebug(fmt::format("Animation {}: {}", i+1, anims[i]->getName().buffer()));
		}
		auto& bones = skeletonData_->getBones();
		for(std::size_t i = 0; i < bones.size(); i++)
		{
			LogDebug(fmt::format("Bones {}: {}", i+1, bones[i]->getName().buffer()));
		}
		auto& pages = atlas_->getPages();
		for(std::size_t i = 0; i < pages.size(); i++)
		{
			LogDebug(fmt::format("Page {}: {}", i+1, pages[i]->name.buffer()));
		}

	}

	void End() override
	{
#ifdef TRACY_ENABLE
		ZoneScoped;
#endif
		skeletonDrawable_ = nullptr;
		delete skeletonData_;
		skeletonJson_ = nullptr;
		attachmentLoader_ = nullptr;
		atlas_ = nullptr;
		textureLoader_ = nullptr;

		RemoveDrawInterface(this);
		RemoveSystem(this);
	}

	void Update(float dt) override
	{
#ifdef TRACY_ENABLE
		ZoneScoped;
#endif
		if(skeletonDrawable_)
		{
			skeletonDrawable_->update(dt, spine::Physics_Update);
		}
	}

	[[nodiscard]] int GetSystemIndex() const override
	{
		return systemIndex_;
	}

	void SetSystemIndex(int index) override
	{
		systemIndex_ = index;
	}

	void Draw() override
	{
		if(skeletonDrawable_)
		{
			auto* renderer = GetRenderer();
			auto windowSize = GetWindowSize();
			skeletonDrawable_->skeleton->setPosition((float)windowSize.first/2,(float)windowSize.second/2);
			skeletonDrawable_->skeleton->setScaleX(0.1f);
			skeletonDrawable_->skeleton->setScaleY(0.1f);
			skeletonDrawable_->draw(renderer);
		}
	}

	void SetGraphicsIndex(int index) override
	{
		graphicsIndex_ = index;
	}

	[[nodiscard]] int GetGraphicsIndex() const override
	{
		return graphicsIndex_;
	}
private:
	std::unique_ptr<spine::Atlas> atlas_;
	std::unique_ptr<spine::SDLTextureLoader> textureLoader_;
	std::unique_ptr<spine::AtlasAttachmentLoader> attachmentLoader_;
	std::unique_ptr<spine::SkeletonJson> skeletonJson_;
	std::unique_ptr<spine::SkeletonDrawable> skeletonDrawable_;
	spine::SkeletonData * skeletonData_ = nullptr;
	int systemIndex_ = -1;
	int graphicsIndex_ = -1;
};
}
int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	splash::Engine engine;
	splash::SpineTest spineTest;
	engine.Run();
	return 0;
}