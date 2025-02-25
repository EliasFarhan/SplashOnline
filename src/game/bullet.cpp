//
// Created by unite on 09.07.2024.
//
#include "game/bullet.h"
#include "game/game_systems.h"
#include "utils/adler32.h"

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
		bullet.colliderUserData.type = ColliderType::BULLET;

		auto& body = physicsWorld.body(bullet.bodyIndex);
		body.isActive = false;
		body.type = neko::BodyType::DYNAMIC;

		auto& collider = physicsWorld.collider(bullet.colliderIndex);
		collider.isTrigger = true;
		collider.userData = &bullet.colliderUserData;

		auto& circle = physicsWorld.circle(collider.shapeIndex);
		circle.radius = Bullet::Radius * Bullet::Scale;
	}
}

void BulletManager::Tick()
{
	auto& physicsWorld = gameSystems_->GetPhysicsWorld();
	for(auto& bullet: bullets_)
	{
		if(!bullet.timeToLiveTimer.Over())
		{
			bullet.timeToLiveTimer.Update(GetFixedDeltaTime());
			auto& body = physicsWorld.body(bullet.bodyIndex);
			if(bullet.timeToLiveTimer.Over())
			{
				body.isActive = false;
				bullet.destroyedTimer.Reset();
			}
			else
			{
				if(bullet.previousPositions[0] != body.position)
				{
					if(bullet.previousPositions.is_full())
					{
						bullet.previousPositions.erase(bullet.previousPositions.cend()-1);
					}
					bullet.previousPositions.insert(bullet.previousPositions.cbegin(), body.position);
				}
			}
		}
		if(!bullet.destroyedTimer.Over())
		{
			bullet.destroyedTimer.Update(GetFixedDeltaTime());
			if(bullet.destroyedTimer.Over())
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
		//Now we found the bullet
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
			bullet.destroyedTimer.Reset();
		}
		break;
	}
}

void BulletManager::SpawnWata(
	neko::Vec2f position,
	neko::Vec2f targetDir,
	int playerNumber,
	bool straight,
	neko::Scalar speedFactor)
{
	auto it = std::find_if(bullets_.begin(), bullets_.end(),[](auto& bullet){
		return bullet.playerNumber == -1;
	});
	Bullet* newBullet = nullptr;
	if(it == bullets_.end())
	{
		neko::Scalar ratio{-1.0f};
		for(auto& bullet : bullets_)
		{
			const auto& body = gameSystems_->GetPhysicsWorld().body(bullet.bodyIndex);
			if(body.isActive) continue;
			const auto currentRatio = bullet.destroyedTimer.CurrentRatio();
			if(ratio < currentRatio)
			{
				newBullet = &bullet;
				ratio = currentRatio;
			}
		}
		if(newBullet == nullptr)
		{
			//We have a problem
			std::terminate();
		}
	}
	else
	{
		newBullet = &*it;
	}
	newBullet->playerNumber = playerNumber;
	newBullet->colliderUserData.playerNumber = playerNumber;
	newBullet->timeToLiveTimer.Reset();
	newBullet->previousPositions.clear();
	auto& bulletBody = gameSystems_->GetPhysicsWorld().body(newBullet->bodyIndex);
	bulletBody.isActive = true;
	bulletBody.type = straight ? neko::BodyType::KINEMATIC : neko::BodyType::DYNAMIC;
	bulletBody.position = position;
	bulletBody.velocity = targetDir * speedFactor * Bullet::WataSpeed;
	newBullet->previousPositions.insert(newBullet->previousPositions.cbegin(), bulletBody.position);
}

Checksum<static_cast<int>(BulletChecksumIndex::LENGTH)> BulletManager::CalculateChecksum() const
{
	Adler32 bulletResult{};
	Adler32 bulletBodyResult{};
	for(const auto & bullet : bullets_)
	{
		bulletResult.Add(bullet.bodyIndex);
		bulletResult.Add(bullet.colliderIndex);
		bulletResult.Add(bullet.destroyedTimer);
		bulletResult.Add(bullet.timeToLiveTimer);
		bulletResult.Add(bullet.colliderUserData);
		bulletResult.Add(bullet.playerNumber);

		const auto& body = gameSystems_->GetPhysicsWorld().body(bullet.bodyIndex);
		bulletBodyResult.Add(body.position);
		bulletBodyResult.Add(body.velocity);
		bulletBodyResult.Add(body.force);
		bulletBodyResult.Add(body.inverseMass);
		bulletBodyResult.Add(body.type);
		bulletBodyResult.Add(body.isActive);
	}
	return {bulletResult.GetValue(), bulletBodyResult.GetValue()};
}

void BulletManager::RollbackFrom(const BulletManager& system)
{
	bullets_ = system.bullets_;
}
}