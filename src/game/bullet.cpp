//
// Created by unite on 09.07.2024.
//
#include "game/bullet.h"
#include "game/game_systems.h"

#include <algorithm>

namespace splash
{

BulletManager::BulletManager(GameSystems* gameSystems) : gameSystems_(gameSystems)
{

}

void BulletManager::Begin()
{
	auto& physicsWorld = gameSystems_->GetPhysicsWorld();
	for(auto& bullet: bullets_)
	{
		bullet.bodyIndex = physicsWorld.AddBody();
		bullet.colliderIndex = physicsWorld.AddCircleCollider(bullet.bodyIndex);

		auto& body = physicsWorld.body(bullet.bodyIndex);
		body.isActive = false;
		body.type = neko::BodyType::DYNAMIC;

		auto& collider = physicsWorld.collider(bullet.colliderIndex);
		collider.isTrigger = true;

		auto& circle = physicsWorld.circle(collider.shapeIndex);
		circle.radius = Bullet::radius * Bullet::scale;
	}
}

void BulletManager::Tick()
{
	for(auto& bullet: bullets_)
	{
		if(!bullet.destroyTimer.Over())
		{
			bullet.destroyTimer.Update(fixedDeltaTime);
			if(bullet.destroyTimer.Over())
			{
				bullet.playerNumber = -1;
			}
		}
	}
}

void BulletManager::End()
{

}

void BulletManager::OnTriggerEnter(neko::ColliderIndex bulletIndex, const neko::Collider& otherCollider)
{
	auto* colliderUserData = static_cast<const ColliderUserData*>(otherCollider.userData);
	if(colliderUserData->type == ColliderType::BULLET)
	{
		return;
	}
	for(auto& bullet: bullets_)
	{
		if(bullet.colliderIndex != bulletIndex)
		{
			continue;
		}
		if(colliderUserData->playerNumber == bullet.playerNumber)
		{
			return;
		}
		auto& body = gameSystems_->GetPhysicsWorld().body(bullet.bodyIndex);
		body.isActive = false;
		if(colliderUserData->type == ColliderType::GAME_LIMIT)
		{
			bullet.playerNumber = -1;
		}
		else
		{
			bullet.destroyTimer.Reset();
		}
		break;
	}
}

void
BulletManager::SpawnWata(neko::Vec2f position, neko::Vec2f targetDir, int playerNumber, bool straight, neko::Scalar speed)
{
	auto it = std::find_if(bullets_.begin(), bullets_.end(),[](auto& bullet){
		return bullet.playerNumber == -1;
	});
	Bullet* bullet = nullptr;
	if(it == bullets_.end())
	{
		//Try to find a bullet with empty body
		it = std::find_if(bullets_.begin(), bullets_.end(),[this](auto& bullet){
			const auto& bulletBody = gameSystems_->GetPhysicsWorld().body(bullet.bodyIndex);
			return !bulletBody.isActive;
		});
		bullet = &*it;
	}
	else
	{
		bullet = &*it;
	}
	bullet->playerNumber = playerNumber;
	auto& bulletBody = gameSystems_->GetPhysicsWorld().body(bullet->bodyIndex);
	bulletBody.isActive = true;
	bulletBody.type = straight ? neko::BodyType::KINEMATIC : neko::BodyType::DYNAMIC;
	bulletBody.position = position;
	bulletBody.velocity = targetDir*speed;
}
}