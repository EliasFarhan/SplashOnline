#include "rollback/rollback_manager.h"
#include "network/packet.h"
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
			static constexpr auto currentFrame = 5_u16;

			rollbackManager.SetInput(0, {player1Input}, 0);

			REQUIRE(rollbackManager.GetLastReceivedFrame() == -1);
			for (uint16_t i = 0; i < currentFrame; i++)
			{
				rollbackManager.SetInput(1_u8, { neko::Fixed8{ 0.5f }}, i);
			}
			REQUIRE(rollbackManager.GetLastReceivedFrame() == 0);
			THEN("The other player as its same input replicated")
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
				inputs[i] = neko::Fixed8{-0.8f+0.25f*static_cast<float>(i)};
				inputPacket.inputs[i] = {inputs[i]};
			}
			inputPacket.inputSize = sixit::guidelines::narrow_cast<uint8_t>(inputs.size());
			inputPacket.playerNumber = 0;
			inputPacket.frame = sixit::guidelines::narrow_cast<uint16_t>(inputs.size())-1;

			rollbackManager.SetInputs(inputPacket);

			THEN("The inputs are stored in the Rollback system")
			{
				for(std::size_t i = 0; i < inputs.size(); i++)
				{
					REQUIRE(inputs[i] == rollbackManager.GetInput(0, static_cast<int>(i)).moveDirX);
				}
			}
		}
	}
}

SCENARIO("Rollback gives a confirm value when confirming")
{
	GIVEN("A rollback system with inputs")
	{
		splash::GameData gameData{{ true, true, false, false }};
		splash::RollbackManager rollbackManager{ gameData };
		static constexpr auto frameCount = 5_u16;
		for(uint16_t i = 0; i < frameCount; i++)
		{
			rollbackManager.SetInput(0_u8, {neko::Fixed8{-0.8f+0.25f*static_cast<float>(i)}}, i);
			rollbackManager.SetInput(1_u8, {neko::Fixed8{0.8f-0.25f*static_cast<float>(i)}}, i);
		}
		REQUIRE(rollbackManager.GetLastReceivedFrame() == frameCount-1);
		REQUIRE(rollbackManager.GetLastConfirmFrame() == -1);

		rollbackManager.Begin();
		WHEN("Confirming the frames")
		{
			int confirmFrameCount = 0;
			while(rollbackManager.GetLastReceivedFrame() > rollbackManager.GetLastConfirmFrame())
			{
				[[maybe_unused]] const auto checksum = rollbackManager.ConfirmLastFrame();
				confirmFrameCount++;
			}
			REQUIRE(confirmFrameCount == frameCount);
			REQUIRE(rollbackManager.GetLastConfirmFrame() == frameCount-1);
		}

	}
}