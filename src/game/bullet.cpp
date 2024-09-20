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
			bullet.timeToLiveTimer.Update(fixedDeltaTime);
			if(bullet.timeToLiveTimer.Over())
			{
				auto& body = physicsWorld.body(bullet.bodyIndex);
				body.isActive = false;
				bullet.destroyedTimer.Reset();
			}
		}
		if(!bullet.destroyedTimer.Over())
		{
			bullet.destroyedTimer.Update(fixedDeltaTime);
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
	auto& bulletBody = gameSystems_->GetPhysicsWorld().body(newBullet->bodyIndex);
	bulletBody.isActive = true;
	bulletBody.type = straight ? neko::BodyType::KINEMATIC : neko::BodyType::DYNAMIC;
	bulletBody.position = position;
	bulletBody.velocity = targetDir * speedFactor * Bullet::WataSpeed;
}

Checksum<1> BulletManager::CalculateChecksum() const
{
	std::uint32_t bulletResult = 0;
	std::uint32_t bulletBodyResult = 0;
	std::uint32_t bulletColliderResult = 0;
	for(const auto & bullet : bullets_)
	{
		const auto* bulletPtr = reinterpret_cast<const std::uint32_t*>(&bullet);
		for(std::size_t j = 0; j < sizeof(Bullet)/sizeof(std::uint32_t); j++)
		{
			bulletResult += bulletPtr[j];
		}
		const auto& body = gameSystems_->GetPhysicsWorld().body(bullet.bodyIndex);
		auto* bodyPtr = reinterpret_cast<const std::uint32_t *>(&body);

		for(std::size_t i = 0; i < sizeof(neko::Body)/sizeof(std::uint32_t); i++)
		{
			if(i*sizeof(std::uint32_t)== offsetof(neko::Body, type))
			{
				bulletBodyResult += (std::uint32_t)body.type;
				bulletBodyResult += body.isActive;
				break;
			}
			bulletBodyResult += (std::uint32_t )bodyPtr[i];

		}
		const auto& collider = gameSystems_->GetPhysicsWorld().collider(bullet.colliderIndex);
		auto* colliderPtr = reinterpret_cast<const std::uint32_t*>(&collider);
		for(std::size_t i = 0; i < sizeof(neko::Collider)/sizeof(std::uint32_t ); i++)
		{
			if(i*sizeof(std::uint32_t) == offsetof(neko::Collider, type))
			{
				bulletBodyResult += (std::uint32_t)body.type;
				bulletBodyResult += body.isActive;
				break;
			}
			bulletBodyResult += colliderPtr[i];
		}
	}
	return {bulletResult, bulletBodyResult, bulletColliderResult};
}

void BulletManager::RollbackFrom(const BulletManager& system)
{
	bullets_ = system.bullets_;
}
}