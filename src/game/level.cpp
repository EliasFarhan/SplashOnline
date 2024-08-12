//
// Created by unite on 08.08.2024.
//

#include "game/level.h"

namespace splash
{

void Level::Begin()
{
	int i = 0;
	for(const auto& platform : platforms)
	{
		auto& pair = platformPhysics_[i];
		pair.first = world_->AddBody();
		pair.second = world_->AddAabbCollider(pair.first);

		auto& body = world_->body(pair.first);
		body.position = platform.position;
		body.type = neko::BodyType::STATIC;
		body.inverseMass = neko::Scalar {0};

		auto& collider = world_->collider(pair.second);
		collider.offset = platform.offset;
		collider.isTrigger = false;
		collider.restitution = neko::Scalar{0};
		auto& aabb = world_->aabb(collider.shapeIndex);
		aabb.halfSize = platform.size/neko::Scalar{2};
		i++;
	}
}
void Level::End()
{

}
Level::Level(neko::PhysicsWorld* world): world_(world)
{

}
}