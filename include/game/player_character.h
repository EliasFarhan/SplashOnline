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
	static constexpr neko::Fixed16 WalkDeadZone{ 0.1f};
	static constexpr neko::Fixed16 InAirForce{ 20.0f };
	static constexpr neko::Fixed16 WaterForce{ 140.0f }; //WHen touch by water on ground, origin 100
	static constexpr neko::Fixed16 MegaForce{ 400.0f };
	static constexpr neko::Fixed16 RainForce{ 45.0f }; //When touch by water from rain
	static constexpr neko::Fixed16 AttackForce{ 5.0f };  //Recoil in air
	static constexpr neko::Fixed16 RecoilGroundFactor{ 0.6f };
	static constexpr neko::Fixed16 WalkSpeed{ 5.0f };
	static constexpr neko::Fixed16 MaxSpeed{ 8.0f };                    // The fastest the player can travel in the x axis.
	static constexpr neko::Fixed16 CapMoveForce{ 50.0f };
	static constexpr neko::Fixed16 WetCapMoveForce{ 20.0f };
	static constexpr neko::Fixed16 ReactorForce { 22.0f };//19.0f
	static constexpr neko::Fixed16 JumpForce{ 320.0f };//437.5
	static constexpr neko::Fixed16 HitEffectPeriod{ 0.02f };
	static constexpr int FirstShotsCount = 3;

	//Respawn
	Timer<> respawnPauseTimer{ neko::Fixed16{ 1.0f }, neko::Fixed16{ 0.5f }};
	Timer<> respawnMoveTimer{ neko::Fixed16{ 1.0f }, neko::Fixed16{ 1.0f }};
	Timer<> respawnStaticTime{ neko::Fixed16{ 2.0f }, neko::Fixed16{ 2.0f }};
	Timer<> invincibleTimer{neko::Fixed16 {-1}, neko::Fixed16 {2.0f}};

	//Wata Hit
	Timer<> hitTimer{neko::Fixed16{-1}, neko::Fixed16{1.5f}};
	neko::Vec2f hitDirection{};
	int resistancePhase = 0; //Used for the resistance to wata bullet

	//Wata shoot
	Timer<> reserveWaterTimer{neko::Fixed16{4.0f}, neko::Fixed16{4.0f}};
	int firstShots = FirstShotsCount;
	Timer<> shootAnimTimer{neko::Fixed16{-1.0f}, neko::Fixed16{0.133f}};
	bool reload = false;
	Timer<> waterTimer{neko::Fixed16{0.3f}, neko::Fixed16{0.2f}};

	Timer<> burstTimer{neko::Fixed16{1}, neko::Fixed16{0.1f}};
	Timer<> jumpTimer{neko::Fixed16{-1}, neko::Fixed16{1.0f}};
	Timer<> jetBurstTimer{neko::Fixed16{-1.0f}, neko::Fixed16{0.5f}};

	int footCount = 0;

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
			{neko::Scalar{playerScale*0.04818996f}, neko::Scalar{playerScale*0.09242455f}},
			{neko::Scalar{playerScale*0.6595958f}, neko::Scalar{playerScale*0.3909828f}}
		};
	ColliderUserData userData{};
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
	void SetPlayerInput(neko::Span<PlayerInput> playerInputs);
private:
	GameSystems* gameSystems_ = nullptr;
	std::array<PlayerInput, MaxPlayerNmb> playerInputs_{};
	std::array<PlayerCharacter, MaxPlayerNmb> playerCharacters_{};
	std::array<PlayerPhysic, MaxPlayerNmb> playerPhysics_{};
};
}
#endif //SPLASHONLINE_PLAYER_CHARACTER_H
