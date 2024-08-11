#ifndef SPLASHONLINE_PLAYER_RENDERER_H_
#define SPLASHONLINE_PLAYER_RENDERER_H_

#include "game/game_systems.h"

#include <spine-sdl-cpp.h>
#include <array>
#include <memory>

namespace splash
{

class PlayerRenderer
{
public:
	explicit PlayerRenderer(const GameSystems* gameSystems);
	void Begin();
	void End();
	void Update(float dt);
	void Tick();
	void Draw();

private:
	const GameSystems* gameSystems_ = nullptr;
	std::array<std::unique_ptr<spine::SkeletonDrawable>, MaxPlayerNmb> playerSkeletonDrawables_{};
};

}

#endif