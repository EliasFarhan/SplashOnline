//
// Created by unite on 08.08.2024.
//

#include "game/level.h"

namespace splash
{



void Level::Begin()
{
	for(std::size_t i = 0; i < platformPhysics_.size(); i++)
	{
		auto& platform = platformPhysics_[i];
		platform.bodyIndex = world_->AddBody();
		platform.colliderIndex = world_->AddAabbCollider(platform.bodyIndex);

		auto& body = world_->body(platform.bodyIndex);
		body.position = platforms[i].position;
		body.type = neko::BodyType::STATIC;
		body.inverseMass = neko::Scalar {0};

		auto& collider = world_->collider(platform.colliderIndex);
		collider.offset = platforms[i].offset;
		collider.isTrigger = false;
		collider.restitution = neko::Scalar{0};
		collider.userData = &platform.userData;
		auto& aabb = world_->aabb(collider.shapeIndex);
		aabb.halfSize = platforms[i].size/neko::Scalar{2};

		platform.userData.type = ColliderType::PLATFORM;

	}

	for(auto& gameLimit : gameLimits)
	{
		const auto bodyIndex = world_->AddBody();
		const auto colliderIndex = world_->AddAabbCollider(bodyIndex);

		auto& body = world_->body(bodyIndex);
		body.position = gameLimit.first.position;
		body.type = neko::BodyType::STATIC;
		body.inverseMass = neko::Scalar {0};

		auto& collider = world_->collider(colliderIndex);
		collider.offset = gameLimit.first.offset;
		collider.isTrigger = false;
		collider.restitution = neko::Scalar{0};
		collider.userData = &gameLimit.second;

		auto& aabb = world_->aabb(collider.shapeIndex);
		aabb.halfSize = gameLimit.first.size/neko::Scalar{2};

	}
}
void Level::End()
{

}
Level::Level(neko::PhysicsWorld* world): world_(world)
{

}
}