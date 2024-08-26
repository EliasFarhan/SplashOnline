//
// Created by unite on 02.08.2024.
//

#include "game/graphics/bullet_view.h"
#include "graphics/graphics_manager.h"
#include "game/game_systems.h"
#include "audio/audio_manager.h"
#include "audio/player_sound.h"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace splash
{

void BulletView::Begin()
{

}

void BulletView::End()
{

}

void BulletView::Update(float dt)
{
	if (!IsSpineLoaded())
		return;
	if(bulletRenderDatas_[0].drawable == nullptr)
	{
		Load();
	}
	const auto& bulletManager = gameSystems_->GetBulletManager();
	const auto& physicsWorld = gameSystems_->GetPhysicsWorld();

	const auto scale = (float)Bullet::Scale * GetGraphicsScale();
	for(int i = 0; i < BulletManager::MaxBulletNmb; i++)
	{
		const auto& bullet = bulletManager.GetBullets()[i];
		auto& bulletRenderData = bulletRenderDatas_[i];
		const auto& body = physicsWorld.body(bullet.bodyIndex);
		switch(bulletRenderData.state)
		{

		case BulletRenderData::BulletRenderState::NONE:
		{
			if(bullet.playerNumber != -1 && body.isActive)
			{
				if(body.type == neko::BodyType::KINEMATIC)
				{
					FmodPlaySound(GetPlayerSoundEvent(PlayerSoundId::DOUBLEGUN));
				}
				else
				{
					FmodPlaySound(GetPlayerSoundEvent(PlayerSoundId::GUN));
				}
				bulletRenderData.state = BulletRenderData::BulletRenderState::WATA;
				bulletRenderData.drawable->animationState->setAnimation(0, "water_fly", true);
				break;
			}
			if(bullet.playerNumber != -1 && !body.isActive && !bullet.destroyedTimer.Over())
			{
				bulletRenderData.state = BulletRenderData::BulletRenderState::DESTROYED;
				bulletRenderData.drawable->animationState->setAnimation(0, "water_destroy", false);
				break;
			}
			continue;
		}
		case BulletRenderData::BulletRenderState::WATA:
		{
			if(bullet.playerNumber == -1)
			{
				bulletRenderData.state = BulletRenderData::BulletRenderState::NONE;
				continue;
			}
			if(!bullet.destroyedTimer.Over())
			{
				FmodPlaySound(GetPlayerSoundEvent(PlayerSoundId::IMPACT1));
				bulletRenderData.state = BulletRenderData::BulletRenderState::DESTROYED;
				bulletRenderData.drawable->animationState->setAnimation(0, "water_destroy", false);

			}


			break;
		}
		case BulletRenderData::BulletRenderState::DESTROYED:
		{
			if(bullet.destroyedTimer.Over())
			{
				bulletRenderData.state = BulletRenderData::BulletRenderState::NONE;
				continue;
			}

			break;
		}
		}
		bulletRenderData.drawable->skeleton->setScaleX(scale);
		bulletRenderData.drawable->skeleton->setScaleY(scale);
		const auto position = GetGraphicsPosition(body.position);
		bulletRenderData.drawable->skeleton->setPosition((float)position.x, (float)position.y);

		bulletRenderData.drawable->update(dt, spine::Physics_Update);

	}
}

void BulletView::Draw()
{
	auto* renderer = GetRenderer();
	const auto& bulletManager = gameSystems_->GetBulletManager();

	const auto& physicsWorld = gameSystems_->GetPhysicsWorld();
	for(int i = 0; i < BulletManager::MaxBulletNmb; i++)
	{
		auto& bulletRenderData = bulletRenderDatas_[i];
		if(bulletRenderData.state == BulletRenderData::BulletRenderState::NONE)
		{
			continue;
		}
		bulletRenderData.drawable->draw(renderer);
		if (GetDebugConfig().showPhysicsBox)
		{
			const auto& bullet = bulletManager.GetBullets()[i];
			const auto& body = physicsWorld.body(bullet.bodyIndex);
			const auto rect = GetDrawingRect(
				body.position,
				neko::Vec2f(Bullet::Radius + Bullet::Radius, Bullet::Radius + Bullet::Radius));
			const auto& color = playerColors[bullet.playerNumber];
			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
			SDL_RenderDrawRect(renderer, &rect);
		}
	}
}

void BulletView::Load()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	for(int i = 0; i < BulletManager::MaxBulletNmb; i++)
	{
		bulletRenderDatas_[i].drawable = CreateSkeletonDrawable(SpineManager::WATA);
		bulletRenderDatas_[i].drawable->animationState->setAnimation(0, "water_fly", true);
	}

}

BulletView::BulletView(const GameSystems* gameSystems): gameSystems_(gameSystems)
{

}
}
