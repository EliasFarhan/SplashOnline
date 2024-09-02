#ifndef SPLASHONLINE_PLAYER_RENDERER_H_
#define SPLASHONLINE_PLAYER_RENDERER_H_

#include "game/game_systems.h"
#include "graphics/spine_manager.h"

#include <spine-sdl-cpp.h>
#include <fmod_studio.hpp>

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

	class VisualFx
	{
	public:
		void Create(SpineManager::SkeletonId skeletonId, std::string_view animName);
		void StartAnim(std::string_view animName, neko::Vec2f position, neko::Vec2<float> scale, float angle = 0.0f);
		void Update(float dt);
		void Draw();
	private:
		Timer<float> animationTimer{-1.0f, 0.0f};
		std::unique_ptr<spine::SkeletonDrawable> drawable{};
		float angle_ = 0.0f;
	};

	VisualFx jetBurstFx;
	VisualFx dashPrepFx;
	VisualFx ejectFx;


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
struct PlayerSoundData
{
	FMOD::Studio::EventInstance* jetpackSoundInstance = nullptr;
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
	std::array<PlayerSoundData, MaxPlayerNmb> playerSoundDatas_{};
};

}

#endif