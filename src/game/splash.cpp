//
// Created by unite on 19.08.2024.
//

#include "game/splash.h"
#include "engine/engine.h"

namespace splash
{

void SplashManager::OnGui()
{

}
void SplashManager::SetGuiIndex(int index)
{
	guiIndex_ = index;
}
int SplashManager::GetGuiIndex() const
{
	return guiIndex_;
}

void SplashManager::Begin()
{

}

void SplashManager::End()
{
	RemoveDrawInterface(this);
	RemoveSystem(this);
	RemoveGuiInterface(this);
}

void SplashManager::Update(float dt)
{
	if(!IsSpineLoaded() || !IsFmodLoaded())
	{
		return;
	}
	if(!logo_ && state_ == State::LOGO)
	{
		logo_ = CreateSkeletonDrawable(SpineManager::KWAKWA_LOGO);
		logo_->animationState->setAnimation(0,"kwakwa",false);
		GetMusicManager().Play();
		logoTimer_.Reset();
		bg_ = GetTexture(TextureManager::TextureId::BG);
	}

	switch(state_)
	{

	case State::LOGO:
	{
		logoTimer_.Update(dt);
		const auto scale = GetGraphicsScale();
		logo_->skeleton->setScaleX(scale);
		logo_->skeleton->setScaleY(scale);

		const auto position = GetGraphicsPosition({});
		logo_->skeleton->setPosition((float)position.x, (float)position.y);
		logo_->update(dt, spine::Physics_Update);
		static bool switchToTitle = false;
		if(logoTimer_.Over())
		{
			state_ = State::LOBBY;
			GetMusicManager().SetParameter("Transition Intro", 0.0f);
			GetMusicManager().SetParameter("Transition Title", 0.5f);
		}
		else
		{
			if(logoTimer_.CurrentTime() > 1.5f)
			{
				if(!switchToTitle)
				{
					switchToTitle = true;
					GetMusicManager().SetParameter("Transition Intro", 0.5f);
				}
				else
				{
					GetMusicManager().SetParameter("Transition Intro", 0.0f);
					GetMusicManager().SetParameter("Transition Title", 0.5f);
				}
			}
		}
		break;
	}
	case State::LOBBY:
	{

		break;

	}
	case State::GAME:
	{
		break;
	}
	case State::VICTORY_SCREEN:
	{
		break;
	}
	}


}

int SplashManager::GetSystemIndex() const
{
	return systemIndex_;
}

void SplashManager::SetSystemIndex(int index)
{
	systemIndex_ = index;
}

void SplashManager::Draw()
{
	if(!logo_)
	{
		return;
	}
	auto* renderer = GetRenderer();
	switch (state_)
	{

	case State::LOGO:
	{
		logo_->draw(renderer);
		break;
	}
	case State::LOBBY:
	{
		constexpr neko::Vec2f size{neko::Scalar{19.2f}, neko::Scalar{10.8f}};
		const auto rect = GetDrawingRect({}, size);
		SDL_RenderCopy(renderer, bg_, nullptr, &rect);
		break;
	}
	case State::GAME:
		break;
	case State::VICTORY_SCREEN:
		break;
	}
}

void SplashManager::SetGraphicsIndex(int index)
{
	graphicsIndex_ = index;
}

int SplashManager::GetGraphicsIndex() const
{
	return graphicsIndex_;
}

SplashManager::SplashManager()
{
	AddDrawInterface(this);
	AddGuiInterface(this);
	AddSystem(this);
}
}