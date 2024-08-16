//
// Created by unite on 09.07.2024.
//

#ifndef SPLASHONLINE_BULLET_H
#define SPLASHONLINE_BULLET_H

#include "utils/timer.h"
#include "game/const.h"

#include <physics/physics.h>

namespace splash
{
struct Bullet
{
	neko::BodyIndex bodyIndex = neko::INVALID_BODY_INDEX;
	neko::ColliderIndex colliderIndex = neko::INVALID_COLLIDER_INDEX;
	Timer<neko::Scalar> destroyedTimer{neko::Scalar{-1}, neko::Scalar{0.2f}};
	ColliderUserData colliderUserData{};
	int playerNumber = -1;

	static constexpr neko::Scalar radius{0.2f};
	static constexpr neko::Scalar scale{0.7f};
};

class GameSystems;
class BulletManager
{
public:
	explicit BulletManager(GameSystems* gameSystems);
	void Begin();
	void Tick();
	void End();

	void SpawnWata(neko::Vec2f position, neko::Vec2f targetDir, int playerNumber, bool straight, neko::Scalar speed);
	static constexpr int MaxBulletNmb = 50;
	void OnTriggerEnter(neko::ColliderIndex bulletIndex, const neko::Collider& otherCollider);

	[[nodiscard]] const auto& GetBullets() const { return bullets_;}
private:
	GameSystems* gameSystems_ = nullptr;
	std::array<Bullet, MaxBulletNmb> bullets_;
};
}
#endif //SPLASHONLINE_BULLET_H
