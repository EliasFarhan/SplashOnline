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
#include "rollback/rollback_system.h"

#include <math/vec2.h>
#include <physics/physics_type.h>

#include <array>

namespace splash
{
struct PlayerCharacter
{
	static constexpr neko::Scalar WalkDeadZone{ 0.1f };
	static constexpr neko::Scalar InAirForce{ 20.0f };
	static constexpr neko::Scalar WaterForce{ 140.0f }; //WHen touch by water on ground, origin 100
	static constexpr neko::Scalar AttackForce{ 5.0f };  //Recoil in air
	static constexpr neko::Scalar RecoilGroundFactor{ 0.6f };
	static constexpr neko::Scalar WalkSpeed{ 5.0f };
	static constexpr neko::Scalar MaxSpeed{ 8.0f }; // The fastest the player can travel in the x axis on the ground
	static constexpr neko::Scalar CapMoveForce{ 50.0f };
	static constexpr neko::Scalar WetCapMoveForce{ 20.0f };
	static constexpr neko::Scalar ReactorForce{ 22.0f };//19.0f
	static constexpr neko::Scalar JumpForce{ 320.0f };//437.5
	static constexpr neko::Scalar HitEffectPeriod{ 0.02f };
	static constexpr int FirstShotsCount = 3;
	static constexpr neko::Fixed8 deadZone{ splash::deadZone };
	static constexpr int MaxResistancePhase = 3;
	static constexpr int MovePriority = 1;
	static constexpr int DashPrepPriority = 2;
	static constexpr int DashPriority = 2;
	static constexpr int DashedPriority = 3;
	static constexpr int CapVelPriority = 2;
	static constexpr int SlowDashPriority = 1;
	static constexpr int JetPackPriority = 1;
	static constexpr int HitPriority = 2;
	static constexpr int FallingPriority = 0;
	static constexpr auto JetBurstThreshold = neko::Scalar{ 0.75f };
	static constexpr auto ReactorThreshold = neko::Scalar{ 0.2f };
	static constexpr auto GroundReactorThreshold = neko::Scalar{ 0.5f };
	static constexpr auto StompThreshold = neko::Scalar{ -0.5f };
	static constexpr auto StompOrBurstMaxVelocity = neko::Scalar{ 12.0f };
	static constexpr neko::Scalar JumpCancelTime{ 0.8f };
	static constexpr neko::Scalar FirstShotFactor{ 5.0f };
	static constexpr neko::Scalar FirstShotRatio{ 1.0f / 4.0f };
	static constexpr neko::Scalar DashSpeed{ 20.0f };
	static constexpr neko::Scalar DashedSpeed{ -30.0f };
	static constexpr neko::Scalar SlowDashForce{ 30.0f };
	static constexpr neko::Vec2f WataOffsetPos{{}, neko::Scalar{ 0.5f }};
	static constexpr neko::Scalar FallingForce{ 5.0f };
	static constexpr neko::Scalar FallingThreshold{ -0.5f };
	static constexpr neko::Scalar decreaseFactor{-0.625f};

	//Respawn
	Timer<neko::Scalar, 500> respawnPauseTimer{ neko::Scalar{ -1.0f }};
	Timer<neko::Scalar, 1000> respawnMoveTimer{ neko::Scalar{ -1.0f }};
	Timer<neko::Scalar, 2000> respawnStaticTime{ neko::Scalar{ 2.0f }};
	Timer<neko::Scalar, 2000> invincibleTimer{ neko::Scalar{ -1 }};
	int fallCount = 0;
	int killCount = 0;

	//Wata Hit
	Timer<neko::Scalar, 1500> hitTimer{ neko::Scalar{ -1 }};
	neko::Vec2f hitDirection{};
	int resistancePhase = 1; //Used for the resistance to wata bullet
	int hitPlayer = -1;

	//Wata shoot
	Timer<neko::Scalar, 4000> reserveWaterTimer{ neko::Scalar{ 4.0f }};
	int firstShots = FirstShotsCount;
	Timer<neko::Scalar, 200> waterTimer{ neko::Scalar{ 0.3f }};
	Timer<neko::Scalar, 1033> reloadTimer{ neko::Scalar{ -1.0f }};

	Timer<neko::Scalar, 100> jetBurstCoolDownTimer{ neko::Scalar{ 1 }};
	Timer<neko::Scalar, 1000> jumpTimer{ neko::Scalar{ -1 }};
	Timer<neko::Scalar, 500> preJetBurstTimer{ neko::Scalar{ -1.0f }};

	//Dash

	static constexpr int trailLength = 22;
	neko::SmallVector<neko::Vec2f, trailLength> dashPositions{};
	Timer<neko::Scalar,150> dashDownTimer{ neko::Scalar{ -1.0f }};
	Timer<neko::Scalar,150> stopDashTimer{ neko::Scalar{ -1.0f }};
	Timer<neko::Scalar,300> slowDashTimer{ neko::Scalar{ -1.0f }};
	Timer<neko::Scalar,150> bounceDashTimer{ neko::Scalar{ -1.0f }};
	Timer<neko::Scalar,500> dashPrepTimer{ neko::Scalar{ -1.0f }};
	Timer<neko::Scalar,500> dashedTimer{ neko::Scalar{ -1.0f }};
	Timer<neko::Scalar,1000> wasDownRecoverTimer{ neko::Scalar{ -1.0f }};
	Timer<neko::Scalar,200> recoilTimer{ neko::Scalar{ -1.0f }};
	neko::Vec2f recoilDirection{};
	Timer<neko::Scalar, 250> collidedTimer{ neko::Scalar{ -1.0f }};
	int collidedPlayer = -1;
	int footCount = 0;

	[[nodiscard]] bool IsRespawning() const
	{
		return 	!respawnPauseTimer.Over() || !respawnMoveTimer.Over() || !respawnStaticTime.Over();
	}

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

	[[nodiscard]] bool IsDashing() const
	{
		return !dashDownTimer.Over();
	}

	[[nodiscard]] bool IsDashPrepping() const
	{
		return !dashPrepTimer.Over();
	}
	[[nodiscard]] bool IsCollided() const
	{
		return !collidedTimer.Over();
	}


	[[nodiscard]] bool IsDashed() const
	{
		return !dashedTimer.Over();
	}

};

static constexpr float playerScale = 0.3f;

class PlayerPhysic final
{
public:
	neko::BodyIndex bodyIndex = neko::INVALID_BODY_INDEX;
	neko::ColliderIndex colliderIndex = neko::INVALID_COLLIDER_INDEX;
	static constexpr Box box
		{
			{},
			{ neko::Scalar{ playerScale * -0.001866817f }, neko::Scalar{ playerScale * 1.066903f }},
			{ neko::Scalar{ playerScale * 1.280593f }, neko::Scalar{ playerScale * 2.113744f }}
		};

	neko::ColliderIndex footColliderIndex = neko::INVALID_COLLIDER_INDEX;
	neko::ColliderIndex headColliderIndex = neko::INVALID_COLLIDER_INDEX;
	neko::ColliderIndex leftColliderIndex = neko::INVALID_COLLIDER_INDEX;
	neko::ColliderIndex rightColliderIndex = neko::INVALID_COLLIDER_INDEX;
	static constexpr Box footBox
		{
			{},
			{ neko::Scalar{ playerScale * 0.04818996f }, neko::Scalar{ playerScale * 0.0f }},
			{ neko::Scalar{ playerScale * 0.6595958f }, neko::Scalar{ playerScale * 0.3909828f }}
		};
	static constexpr Box headBox
		{
			{{}, neko::Scalar{ 2.14f * playerScale }},
			{ neko::Scalar{ playerScale * -0.00925f }, neko::Scalar{ playerScale * 0.08f }},
			{ neko::Scalar{ playerScale * 1.405238f }, neko::Scalar{ playerScale * 1.0f }}
		};
	static constexpr Box rightBox
		{
			{ neko::Scalar{ -0.31f * playerScale }, {}},
			{ neko::Scalar{ playerScale * 0.8151992f }, neko::Scalar{ playerScale * 0.9742619f }},
			{ neko::Scalar{ playerScale * 0.4565361f }, neko::Scalar{ playerScale * 1.331382f }}
		};
	static constexpr Box leftBox
		{
			{ neko::Scalar{ -1.35f * playerScale }, {}},
			{ neko::Scalar{ playerScale * 0.8151992f }, neko::Scalar{ playerScale * 0.9742619f }},
			{ neko::Scalar{ playerScale * 0.4565361f }, neko::Scalar{ playerScale * 1.331382f }}
		};
	ColliderUserData userData{};


	void AddForce(neko::Vec2f force, int priority)
	{
		if(priority_ < priority)
		{
			totalForce_ = force;
			priority_ = priority;
		}
		else if(priority_ == priority)
		{
			totalForce_ += force;
		}
	}
	void Reset()
	{
		totalForce_ = {};
		priority_ = 0;
	}

	[[nodiscard]] neko::Vec2f GetForce() const { return totalForce_;}
	[[nodiscard]] int GetPriority() const {return priority_;}
private:
	neko::Vec2f totalForce_{};
	int priority_ = 0;

};

class GameSystems;

enum PlayerChecksumIndex
{
	PLAYER_CHARACTER,
	PLAYER_PHYSICS,
	PLAYER_INPUT,
	PLAYER_PREVIOUS_INPUT,
	PLAYER_BODIES,
	LENGTH
};

class PlayerManager : public RollbackInterface<PlayerManager, static_cast<int>(PlayerChecksumIndex::LENGTH)>
{
public:
	explicit PlayerManager(GameSystems* gameSystems);

	void Begin();
	void Tick();
	void End();

	void OnTriggerEnter(neko::ColliderIndex playerIndex, int playerNumber, const neko::Collider& otherCollider);
	void OnTriggerExit(neko::ColliderIndex playerIndex, int playerNumber, const neko::Collider& otherCollider);

	[[nodiscard]] const auto& GetPlayerCharacter() const
	{
		return playerCharacters_;
	}
	[[nodiscard]] const auto& GetPlayerPhysics() const
	{
		return playerPhysics_;
	}
	[[nodiscard]] const auto& GetPlayerInputs() const
	{
		return playerInputs_;
	}
	void SetPlayerInput(neko::Span<PlayerInput> playerInputs);
	void SetPreviousPlayerInput(neko::Span<PlayerInput> playerInputs);

	[[nodiscard]] Checksum<static_cast<int>(PlayerChecksumIndex::LENGTH)> CalculateChecksum() const override;

	void RollbackFrom(const PlayerManager& system) override;

	static constexpr std::array<neko::Vec2f, MaxPlayerNmb> spawnPositions
		{{
			 { neko::Scalar{ -4.77f }, neko::Scalar{ -1.79f }},
			 { neko::Scalar{ 4.13f }, neko::Scalar{ -1.79f }},
			 { neko::Scalar{ -1.65f }, neko::Scalar{ 0.96f }},
			 { neko::Scalar{ 1.38f }, neko::Scalar{ 0.96f }},
		 }};
private:
	GameSystems* gameSystems_ = nullptr;
	std::array<PlayerInput, MaxPlayerNmb> playerInputs_{};
	std::array<PlayerInput, MaxPlayerNmb> previousPlayerInputs_{};
	std::array<PlayerCharacter, MaxPlayerNmb> playerCharacters_{};
	std::array<PlayerPhysic, MaxPlayerNmb> playerPhysics_{};

	void Respawn(int playerNumber);
	enum class DashFinishType
	{
		BOUNCE,
		SLOW,
		NONE
	};
	void StopDash(int playerNumber, DashFinishType bounce);
};
}
#endif //SPLASHONLINE_PLAYER_CHARACTER_H
