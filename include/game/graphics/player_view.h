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
	std::unique_ptr<spine::SkeletonDrawable> cloudDrawable{};
	spine::Bone* shoulderBone{};
	spine::Bone* handBone{};

	SDL_Texture* outIconBg = nullptr;
	SDL_Texture* outIconCharFace = nullptr;
	SDL_Texture* outIconArrow = nullptr;

	PlayerRenderState state = PlayerRenderState::IDLE;
	Timer<float> cloudEndRespawnTimer{-1.0f, 0.367f};
	bool faceRight = true;
	bool isRespawning = true;
	bool wasShooting = false;

	static constexpr auto invincibleFlashPeriod = 0.125f;
	neko::Vec2<float> targetDir{0.0f,-1.0f};
};

class PlayerView
{
public:
	explicit PlayerView(const GameSystems* gameSystems);
	void Begin();
	void End();
	void Update(float dt);
	void Draw();

private:
	void UpdateTransforms(float dt);
	void Load();
	void SwitchToState(PlayerRenderState state, int playerNumber);
	const GameSystems* gameSystems_ = nullptr;

	std::array<PlayerRenderData, MaxPlayerNmb> playerRenderDatas_{};
};

}

#endif