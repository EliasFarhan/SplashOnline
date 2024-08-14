#ifndef SPLASHONLINE_PLAYER_RENDERER_H_
#define SPLASHONLINE_PLAYER_RENDERER_H_

#include "game/game_systems.h"

#include <spine-sdl-cpp.h>
#include <array>
#include <memory>

namespace splash
{

enum class PlayerRenderState
{
	IDLE,
	WALK,
	WALK_BACK,
	SHOOT,
	JET,
	JETBURST,
	FALL,
	DASHPREP,
	DASH,
	BOUNCE,
	LENGTH
};

struct PlayerRenderData
{
	std::unique_ptr<spine::SkeletonDrawable> bodyDrawable{};
	std::unique_ptr<spine::SkeletonDrawable> armDrawable{};
	std::unique_ptr<spine::SkeletonDrawable> gunDrawable{};
	spine::Bone* shoulderBone{};
	spine::Bone* handBone{};
	PlayerRenderState state = PlayerRenderState::IDLE;
	bool faceRight = true;
};

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
	void Load();
	void SwitchToState(PlayerRenderState state, int playerNumber);
	const GameSystems* gameSystems_ = nullptr;

	std::array<PlayerRenderData, MaxPlayerNmb> playerRenderDatas_{};
};

}

#endif