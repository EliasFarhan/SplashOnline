//
// Created by unite on 15.09.2024.
//

#include "engine/engine.h"
#include "game/game_manager.h"
#include "graphics/graphics_manager.h"
#include "utils/log.h"

#include <fmt/format.h>
#include <SDL_main.h>
#include <SDL_ttf.h>

class TextRenderingSystem : public splash::DrawInterface, public splash::SystemInterface
{
public:
	explicit TextRenderingSystem()
	{
		splash::AddSystem(this);
		splash::AddDrawInterface(this);
	}
	void Draw() override
	{
		if(caption_ == nullptr)
			return;
		auto* renderer = splash::GetRenderer();
		SDL_RenderCopy(renderer,caption_, nullptr, &captionRect_);
	}

	void SetGraphicsIndex(int index) override
	{
		graphicsIndex_ = index;
	}

	[[nodiscard]] int GetGraphicsIndex() const override
	{
		return graphicsIndex_;
	}

	void Begin() override
	{
		if (TTF_Init() == -1)
		{
			splash::LogError(fmt::format("Couldn't initialize TTF: {}",SDL_GetError()));
			return;
		}
		static constexpr std::string_view fontName = "data/font/maassslicerItalic.ttf";
		font_ = TTF_OpenFont(fontName.data(), 18);
		if(font_ == nullptr)
		{
			splash::LogError(fmt::format("Could not load font: {}, error: {}", fontName, SDL_GetError()));
			return;
		}
		text_ = TTF_RenderText_Solid(font_, "Hello World!", {100,100,100,255});

		auto* renderer = splash::GetRenderer();
		caption_ = SDL_CreateTextureFromSurface(renderer, text_);
		captionRect_ = SDL_Rect{0,0,text_->w, text_->h};

	}

	void End() override
	{
		TTF_CloseFont(font_);
		TTF_Quit();
		splash::RemoveSystem(this);
		splash::RemoveDrawInterface(this);
	}

	void Update([[maybe_unused]] float dt) override
	{

	}

	[[nodiscard]] int GetSystemIndex() const override
	{
		return systemIndex_;
	}

	void SetSystemIndex(int index) override
	{
		systemIndex_ = index;
	}

private:
	TTF_Font* font_ = nullptr;
	SDL_Surface* text_ = nullptr;
	SDL_Texture* caption_ = nullptr;
	SDL_Rect captionRect_{};


	int graphicsIndex_ = -1;
	int systemIndex_ = -1;
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	splash::AudioManager audioManager;
	TextRenderingSystem textRenderingSystem;
	splash::RunEngine();
	return 0;
}