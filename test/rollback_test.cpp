#include "rollback/rollback_manager.h"
#include "game/game_manager.h"
#include <catch2/catch_test_macros.hpp>

SCENARIO("Rollback predicts inputs by replicating them", "[rollback]")
{
	GIVEN("The Rollback system has some inputs")
	{
		splash::GameData gameData{{ true, true, false, false }};
		splash::RollbackManager rollbackManager{ gameData };

		REQUIRE(rollbackManager.GetLastReceivedFrame() == -1);

		WHEN("A player has several frames in advance")
		{
			static constexpr neko::Fixed8 player1Input{neko::Fixed8{ -0.5f }};
			static constexpr int currentFrame = 5;

			rollbackManager.SetInput(0, {player1Input}, 0);

			REQUIRE(rollbackManager.GetLastReceivedFrame() == -1);
			for (int i = 0; i < currentFrame; i++)
			{
				rollbackManager.SetInput(1, { neko::Fixed8{ 0.5f }}, i);
			}
			REQUIRE(rollbackManager.GetLastReceivedFrame() == 0);
			THEN("The other player as its same input")
			{
				const auto input = rollbackManager.GetInput(0, currentFrame);
				REQUIRE(input.moveDirX == player1Input);
			}
		}

	}
}

SCENARIO("Rollback can receive an InputPacket")
{
	GIVEN("An empty Rollback System")
	{
		splash::GameData gameData{{ true, true, false, false }};
		splash::RollbackManager rollbackManager{ gameData };
		WHEN("Receiving a InputPacket")
		{
			std::array<neko::Fixed8, 5> inputs;
			splash::InputPacket inputPacket{};
			for(std::size_t i = 0; i < inputs.size(); i++)
			{
				inputs[i] = neko::Fixed8{-0.8f+0.25f*(float)i};
				inputPacket.inputs[i] = {inputs[i]};
			}
			inputPacket.inputSize = (int)inputs.size();
			inputPacket.playerNumber = 0;
			inputPacket.frame = (int)inputs.size()-1;

			rollbackManager.SetInputs(inputPacket);

			THEN("The inputs are stored in the Rollback system")
			{
				for(std::size_t i = 0; i < inputs.size(); i++)
				{
					REQUIRE(inputs[i] == rollbackManager.GetInput(0, (int)i).moveDirX);
				}
			}
		}
	}
}