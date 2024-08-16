//
// Created by unite on 06.07.2024.
//

#ifndef SPLASHONLINE_PLAYER_CHARACTER_H
#define SPLASHONLINE_PLAYER_CHARACTER_H

#include "utils/timer.h"
#include "game/const.h"
#include "engine/input_manager.h"
#include "container/span.h"
#include "physics/physics.h"

#include <math/vec2.h>
#include <physics/physics_type.h>

#include <array>

namespace splash
{
struct PlayerCharacter
{
	static constexpr neko::Scalar WalkDeadZone{ 0.1f};
	static constexpr neko::Scalar InAirForce{ 20.0f };
	static constexpr neko::Scalar WaterForce{ 140.0f }; //WHen touch by water on ground, origin 100
	static constexpr neko::Scalar MegaForce{ 400.0f };
	static constexpr neko::Scalar RainForce{ 45.0f }; //When touch by water from rain
	static constexpr neko::Scalar AttackForce{ 5.0f };  //Recoil in air
	static constexpr neko::Scalar RecoilGroundFactor{ 0.6f };
	static constexpr neko::Scalar WalkSpeed{ 5.0f };
	static constexpr neko::Scalar MaxSpeed{ 8.0f };                    // The fastest the player can travel in the x axis.
	static constexpr neko::Scalar CapMoveForce{ 50.0f };
	static constexpr neko::Scalar WetCapMoveForce{ 20.0f };
	static constexpr neko::Scalar ReactorForce { 22.0f };//19.0f
	static constexpr neko::Scalar JumpForce{ 320.0f };//437.5
	static constexpr neko::Scalar HitEffectPeriod{ 0.02f };
	static constexpr int FirstShotsCount = 3;
	static constexpr neko::Fixed8 deadZone {InputManager::deadZone};
	static constexpr int MaxResistancePhase = 3;
	static constexpr int MovePriority = 1;
	static constexpr int JetPackPriority = 1;
	static constexpr auto JetBurstThreshold = neko::Scalar { 0.75f};
	static constexpr auto ReactorThreshold = neko::Scalar { 0.2f};
	static constexpr auto GroundReactorThreshold = neko::Scalar { 0.5f};
	static constexpr neko::Scalar JumpCancelTime{0.8f};
	static constexpr neko::Scalar FirstShotFactor{5.0f};
	static constexpr neko::Scalar FirstShotRatio{1.0f/4.0f};
	static constexpr neko::Vec2f WataOffsetPos{{},neko::Scalar{0.5f}};

	//Respawn
	Timer<> respawnPauseTimer{ neko::Scalar{ -1.0f }, neko::Scalar{ 0.5f }};
	Timer<> respawnMoveTimer{ neko::Scalar{ -1.0f }, neko::Scalar{ 1.0f }};
	Timer<> respawnStaticTime{ neko::Scalar{ 2.0f }, neko::Scalar{ 2.0f }};
	Timer<> invincibleTimer{neko::Scalar {-1}, neko::Scalar {2.0f}};

	//Wata Hit
	Timer<> hitTimer{neko::Scalar{-1}, neko::Scalar{1.5f}};
	neko::Vec2f hitDirection{};
	int resistancePhase = 0; //Used for the resistance to wata bullet

	//Wata shoot
	Timer<> reserveWaterTimer{neko::Scalar{4.0f}, neko::Scalar{4.0f}};
	int firstShots = FirstShotsCount;
	Timer<> waterTimer{neko::Scalar{0.3f}, neko::Scalar{0.2f}};
	Timer<> reloadTimer{neko::Scalar {-1.0f}, neko::Scalar{1.033f}};

	Timer<> jetBurstCoolDownTimer{neko::Scalar{1}, neko::Scalar{0.1f}};
	Timer<> jumpTimer{neko::Scalar{-1}, neko::Scalar{1.0f}};
	Timer<> preJetBurstTimer{neko::Scalar{-1.0f}, neko::Scalar{0.5f}};

	int footCount = 0;

	[[nodiscard]] bool IsGrounded() const
	{
		return footCount > 0;
	}

	[[nodiscard]] bool IsJetBursting() const
	{
		return !preJetBurstTimer.Over();
	}

	[[nodiscard]] bool IsReloading() const
	{
		return !reloadTimer.Over();
	}

};


static constexpr float playerScale = 0.3f;

struct PlayerPhysic
{
	neko::BodyIndex bodyIndex = neko::INVALID_BODY_INDEX;
	neko::ColliderIndex colliderIndex = neko::INVALID_COLLIDER_INDEX;
	static constexpr Box box
		{
			{},
			{neko::Scalar{playerScale*-0.001866817f}, neko::Scalar{playerScale*1.066903f}},
			{neko::Scalar{playerScale*1.280593f}, neko::Scalar{playerScale*2.113744f}}
		};

	neko::ColliderIndex footColliderIndex = neko::INVALID_COLLIDER_INDEX;
	static constexpr Box footBox
		{
			{},
			{neko::Scalar{playerScale*0.04818996f}, neko::Scalar{playerScale*0.0f}},
			{neko::Scalar{playerScale*0.6595958f}, neko::Scalar{playerScale*0.3909828f}}
		};
	ColliderUserData userData{};
	neko::Vec2f totalForce{};
	int priority = 0;

};

class GameSystems;
class PlayerManager
{
public:
	explicit PlayerManager(GameSystems* gameSystems);

	void Begin();
	void Tick();
	void End();

	void OnTriggerEnter(neko::ColliderIndex playerIndex, int playerNumber, const neko::Collider& otherCollider);
	void OnTriggerExit(neko::ColliderIndex playerIndex, int playerNumber, const neko::Collider& otherCollider);

	[[nodiscard]] const auto& GetPlayerCharacter() const {return playerCharacters_;}
	[[nodiscard]] const auto& GetPlayerPhysics()const {return playerPhysics_;}
	[[nodiscard]] const auto& GetPlayerInputs() const { return playerInputs_; }
	void SetPlayerInput(neko::Span<PlayerInput> playerInputs);
	static constexpr std::array<neko::Vec2f, MaxPlayerNmb> spawnPositions
		{{
			 {neko::Scalar{-4.77f}, neko::Scalar{-1.79f}},
			 {neko::Scalar{4.13f}, neko::Scalar{-1.79f}},
			 {neko::Scalar{-1.65f}, neko::Scalar{0.96f}},
			 {neko::Scalar{1.38f}, neko::Scalar{0.96f}},
		 }};
private:
	GameSystems* gameSystems_ = nullptr;
	std::array<PlayerInput, MaxPlayerNmb> playerInputs_{};
	std::array<PlayerCharacter, MaxPlayerNmb> playerCharacters_{};
	std::array<PlayerPhysic, MaxPlayerNmb> playerPhysics_{};

	void Respawn(int playerNumber);
};
}
#endif //SPLASHONLINE_PLAYER_CHARACTER_H
