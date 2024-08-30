//
// Created by unite on 09.07.2024.
//

#ifndef SPLASHONLINE_BULLET_H
#define SPLASHONLINE_BULLET_H

#include "utils/timer.h"
#include "game/const.h"
#include "rollback/rollback_system.h"

#include <physics/physics.h>

namespace splash
{
struct Bullet
{
	neko::BodyIndex bodyIndex = neko::INVALID_BODY_INDEX;
	neko::ColliderIndex colliderIndex = neko::INVALID_COLLIDER_INDEX;
	Timer<neko::Scalar> destroyedTimer{neko::Scalar{-1}, neko::Scalar{0.2f}};
	Timer<neko::Scalar> timeToLiveTimer{neko::Scalar{3.0f}, neko::Scalar{3.0f}};
	ColliderUserData colliderUserData{};
	int playerNumber = -1;

	static constexpr neko::Scalar Radius{ 0.2f};
	static constexpr neko::Scalar Scale{ 0.7f};
	static constexpr neko::Scalar WataSpeed{20.0f};
};

class GameSystems;
class BulletManager : public RollbackInterface<BulletManager, 1>
{
public:
	explicit BulletManager(GameSystems* gameSystems);
	void Begin();
	void Tick();
	void End();

	void SpawnWata(neko::Vec2f position, neko::Vec2f targetDir, int playerNumber, bool straight, neko::Scalar speedFactor);
	static constexpr int MaxBulletNmb = 50;
	void OnTriggerEnter(neko::ColliderIndex bulletIndex, const neko::Collider& otherCollider);

	[[nodiscard]] const auto& GetBullets() const { return bullets_;}
	[[nodiscard]] Checksum<1> CalculateChecksum() const override;

	void RollbackFrom(const BulletManager& system) override;

private:
	GameSystems* gameSystems_ = nullptr;
	std::array<Bullet, MaxBulletNmb> bullets_{};
};
}
#endif //SPLASHONLINE_BULLET_H
