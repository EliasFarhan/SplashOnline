
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
	}

	void End() override
	{
		RemoveDrawInterface(this);
		RemoveSystem(this);
	}

	void Update(float dt) override
	{
#ifdef TRACY_ENABLE
		ZoneScoped;
#endif
		if (!IsSpineLoaded())
			return;
		if (skeletonDrawable_)
		{
			skeletonDrawable_->update(dt, spine::Physics_Update);
		}
		else
		{
			skeletonDrawable_ = CreateSkeletonDrawable(SpineManager::CAT_NOARM);
			skeletonDrawable_->animationState->setAnimation(0, "idle", true);
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
	std::unique_ptr<spine::SkeletonDrawable> skeletonDrawable_;
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