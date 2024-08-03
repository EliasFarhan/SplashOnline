//
// Created by unite on 03.08.2024.
//

#include "engine/system.h"
#include "engine/engine.h"
#include "graphics/graphics_manager.h"

#include <SDL_main.h>
#include <imgui.h>

namespace splash
{
class InputTestSystem : public SystemInterface, public OnGuiInterface
{
public:
	InputTestSystem()
	{
		AddSystem(this);
		AddGuiInterface(this);
	}
	void Begin() override
	{

	}

	void End() override
	{
		RemoveGuiInterface(this);
		RemoveSystem(this);
	}

	void Update([[maybe_unused]]float dt) override
	{
		playerInput_ = GetPlayerInput();
	}

	[[nodiscard]] int GetSystemIndex() const override
	{
		return engineIndex;
	}

	void SetSystemIndex(int index) override
	{
		engineIndex = index;
	}


	void SetGuiIndex(int index) override
	{
		graphicsIndex = index;
	}

	[[nodiscard]] int GetGuiIndex() const override
	{
		return graphicsIndex;
	}

	void OnGui() override
	{
		ImGui::Begin("Input Test");
		ImGui::LabelText("Input", "(%f,%f), (%f, %f) ",
			(float)playerInput_.moveDirX,
			(float)playerInput_.moveDirY,
			(float)playerInput_.targetDirX,
			(float)playerInput_.targetDirY
			);
		ImGui::End();
	}

private:
	int engineIndex = 0;
	int graphicsIndex = 0;
	PlayerInput playerInput_{};
};
}
int main([[maybe_unused]]int argc, [[maybe_unused]]char** argv)
{
	splash::Engine engine{};
	splash::InputTestSystem inputTestSystem{};
	engine.Run();

	return 0;
}