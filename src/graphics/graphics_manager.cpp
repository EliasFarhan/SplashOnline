//
// Created by unite on 12.07.2024.
//

#include "graphics/graphics_manager.h"
#include "engine/window.h"
#include "utils/log.h"
#include "graphics/const.h"

#include <fmt/format.h>

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace splash
{

void GraphicsManager::Begin()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	auto* window = GetWindow();
	renderer_ = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if(renderer_ == nullptr)
	{
		LogError(fmt::format("SDL renderer failed to initialise: {}\n", SDL_GetError()));
		std::terminate();
	}

	AddEventListener(this);

	SDL_GetWindowSize(window, &windowSize_.x, &windowSize_.y);
	ReloadDrawingSize();

	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer_, &info);

	LogDebug(fmt::format("Renderer: {}", info.name));

	textureManager_.Begin();
	guiRenderer_.Begin();
}

void GraphicsManager::End()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	guiRenderer_.End();
	textureManager_.End();
	SDL_DestroyRenderer(renderer_);
}

static GraphicsManager* instance = nullptr;

GraphicsManager::GraphicsManager()
{
	instance = this;
}

SDL_Renderer* GraphicsManager::GetRenderer() const
{
	return renderer_;
}

SDL_Renderer* GetRenderer()
{
	return instance->GetRenderer();
}

void AddDrawInterface(DrawInterface* drawInterface)
{
	instance->AddDrawInterface(drawInterface);
}

void RemoveDrawInterface(DrawInterface* drawInterface)
{
	instance->RemoveDrawInterface(drawInterface);
}

SDL_Rect GetDrawingRect(neko::Vec2f position, neko::Vec2f size)
{
	return instance->GetDrawingRect(position, size);
}

float GetGraphicsScale()
{
	return instance->GetScale();
}

neko::Vec2i GetGraphicsPosition(neko::Vec2f position)
{
	return instance->GetGraphicsPosition(position);
}

neko::Vec2i GetActualGameSize()
{
	return instance->GetActualGameSize();
}

void GraphicsManager::Update([[maybe_unused]]float dt)
{
	if(!textureManager_.IsLoaded())
	{
		textureManager_.UpdateLoad();
	}
	if(!spineManager_.IsLoaded())
	{
		spineManager_.UpdateLoad();
	}
	guiRenderer_.Update();
}

void GraphicsManager::Draw()
{
	for(auto* drawInterface: drawInterfaces_)
	{
		if(drawInterface == nullptr) continue;
		drawInterface->Draw();
	}
	SDL_SetRenderDrawColor(renderer_,0,0,0, 255);
	if(actualSize_.x < windowSize_.x)
	{
		SDL_Rect rect{0,0,offset_.x, windowSize_.y};
		SDL_RenderFillRect(renderer_, &rect);
		rect = {offset_.x+ actualSize_.x, 0, offset_.x, windowSize_.y};
		SDL_RenderFillRect(renderer_, &rect);
	}
	else if(actualSize_.y < windowSize_.y)
	{
		SDL_Rect rect{0,0,windowSize_.x, offset_.y};
		SDL_RenderFillRect(renderer_, &rect);
		rect = {0,offset_.y+ actualSize_.y, windowSize_.x, offset_.y};
		SDL_RenderFillRect(renderer_, &rect);
	}
	guiRenderer_.Draw();
}
void GraphicsManager::PreDraw()
{
	//Clear screen

	SDL_SetRenderDrawColor(renderer_,0,0,0, 255);
	SDL_RenderClear(renderer_);
}
void GraphicsManager::PostDraw()
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	// Rendering
	SDL_RenderPresent(renderer_);
}
void GraphicsManager::AddDrawInterface(DrawInterface* drawInterface)
{
	int index = -1;
	auto it = std::find(drawInterfaces_.begin(), drawInterfaces_.end(), nullptr);
	if(it == drawInterfaces_.end())
	{
		index = static_cast<int>(drawInterfaces_.size());
		drawInterfaces_.push_back(drawInterface);
	}
	else
	{
		*it = drawInterface;
		index = static_cast<int>(std::distance(drawInterfaces_.begin(), it));
	}

	drawInterface->SetGraphicsIndex(index);
}
void GraphicsManager::RemoveDrawInterface(DrawInterface* drawInterface)
{
	drawInterfaces_[drawInterface->GetGraphicsIndex()] = nullptr;
}
SDL_Rect GraphicsManager::GetDrawingRect(neko::Vec2f position, neko::Vec2f size) const
{
	const auto newPosition = neko::Vec2<float>( static_cast<float>(position.x),  static_cast<float>(-position.y))*pixelPerMeter*scale_;
	const auto newSize = neko::Vec2<float>( static_cast<float>(size.x),  static_cast<float>(size.y))*pixelPerMeter*scale_;

	const auto pixelSize = neko::Vec2i(newSize);
	const auto pixelPosition = neko::Vec2i(newPosition)+offset_+actualSize_/2-pixelSize/2;
	return SDL_Rect{pixelPosition.x, pixelPosition.y, pixelSize.x, pixelSize.y};
}

int GraphicsManager::GetEventListenerIndex() const
{
	return eventListenerIndex;
}
void GraphicsManager::SetEventListenerIndex(int index)
{
	eventListenerIndex = index;
}
void GraphicsManager::OnEvent(const SDL_Event& event)
{
	if (event.type == SDL_WINDOWEVENT)
	{
		if(event.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			windowSize_ = {event.window.data1, event.window.data2};
			ReloadDrawingSize();
		}
	}
}
void GraphicsManager::ReloadDrawingSize()
{
	constexpr auto aspectRatio = static_cast<float>(gameWindowSize.x)/static_cast<float>(gameWindowSize.y);
	const auto newAspectRatio =  static_cast<float>(windowSize_.x)/ static_cast<float>(windowSize_.y);
	if(neko::Abs(newAspectRatio-aspectRatio) < 0.001f)
	{
		//Same aspect ratio
		actualSize_ = windowSize_;
		offset_ = {};
	}
	else
	{
		//Add two black bars
		if(newAspectRatio > aspectRatio)
		{
			//vertical black bars
			offset_ = {(windowSize_.x-static_cast<int>( static_cast<float>(windowSize_.y)*aspectRatio))/2, 0};
			actualSize_ = {windowSize_.x-offset_.x*2, windowSize_.y};
		}
		else
		{
			//horizontal black bars
			offset_ = {0,(windowSize_.y-static_cast<int>( static_cast<float>(windowSize_.x)/aspectRatio))/2};
			actualSize_ = {windowSize_.x, windowSize_.y-offset_.y*2};
		}
	}
	scale_ =  static_cast<float>(actualSize_.y)/static_cast<float>(gameWindowSize.y);

}

float GraphicsManager::GetScale() const
{
	return scale_;
}

neko::Vec2i GraphicsManager::GetGraphicsPosition(neko::Vec2f position) const
{
	const auto newPosition = neko::Vec2<float>( static_cast<float>(position.x),  static_cast<float>(-position.y))*pixelPerMeter*scale_;
	return neko::Vec2i(newPosition)+offset_+actualSize_/2;
}

neko::Vec2i GetOffset()
{
	return instance->GetOffset();
}

}