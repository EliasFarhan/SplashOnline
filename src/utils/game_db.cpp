//
// Created by unite on 20.09.2024.
//

#ifdef ENABLE_DESYNC_DEBUG
#include "utils/game_db.h"
#include "engine/engine.h"
#include "utils/log.h"

#include <thread/job_system.h>
#include <fmt/format.h>
#include <sqlite3.h>

#include <memory>

#include <filesystem>
namespace fs = std::filesystem;

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace splash
{

static sqlite3* db_ = nullptr;
static std::unique_ptr<neko::FuncJob> openJob_{};
static std::unique_ptr<neko::FuncJob> confirmFrameJob_{};
static std::unique_ptr<neko::FuncJob> localInputJob_{};

void OpenDatabase(int playerNumber)
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif

	openJob_ = std::make_unique<neko::FuncJob>([playerNumber](){
#ifdef TRACY_ENABLE
		ZoneNamedN(dbOpen, "Create Database", true);
#endif

		const auto dbName = fmt::format("splash_p{}.db", playerNumber);
		if(fs::exists(dbName))
		{
			fs::remove(dbName);
		}
		auto result = sqlite3_open(dbName.data(), &db_);
		if(result != SQLITE_OK)
		{
			LogError(fmt::format("Could not open db: {}", dbName));
		}
		constexpr auto createLocalInputTable = "CREATE TABLE local_inputs("
								  "id INTEGER PRIMARY KEY,"
								  "frame INTEGER NOT NULL,"
								  "move_x INTEGER  NOT NULL,"
								  "move_y INTEGER  NOT NULL,"
								  "target_x INTEGER NOT NULL,"
								  "target_y INTEGER NOT NULL,"
								  "button INTEGER NOT NULL"
								  ");";
		char* errorMsg = nullptr;
		result = sqlite3_exec(db_, createLocalInputTable, nullptr, nullptr, &errorMsg);
		if(result != SQLITE_OK)
		{
			LogError(fmt::format("Could not create table local_inputs: {}", errorMsg));
			sqlite3_free(errorMsg);
			return;
		}

		constexpr auto createRemoteInputTable = "CREATE TABLE remote_inputs("
									  "id INTEGER PRIMARY KEY,"
									  "remote_frame INTEGER NOT NULL,"
									  "local_frame INTEGER NOT NULL,"
									  "player_number INTEGER NOT NULL,"
									  "move_x INTEGER NOT NULL,"
									  "move_y INTEGER NOT NULL,"
									  "target_x INTEGER NOT NULL,"
									  "target_y INTEGER NOT NULL,"
									  "button INTEGER NOT NULL"
									  ");";
		result = sqlite3_exec(db_, createRemoteInputTable, nullptr, nullptr, &errorMsg);
		if(result != SQLITE_OK)
		{
			LogError(fmt::format("Could not create table remote_inputs: {}", errorMsg));
			sqlite3_free(errorMsg);
			return;
		}

		constexpr auto createConfirmFrameTable = "CREATE TABLE confirm_frame("
												"id INTEGER PRIMARY KEY,"
												"confirm_frame INTEGER NOT NULL,"
												"player_character_checksum INTEGER  NOT NULL,"
												"player_physics_checksum INTEGER  NOT NULL,"
												"player_input_checksum INTEGER NOT NULL,"
												"player_previous_input_checksum INTEGER NOT NULL,"
												"player_bodies_checksum INTEGER NOT NULL,"
												"bullets_checksum INTEGER NOT NULL,"
												"bullet_bodies_checksum INTEGER NOT NULL,"
												"final_checksum INTEGER NOT NULL"
												");";
		result = sqlite3_exec(db_, createConfirmFrameTable, nullptr, nullptr, &errorMsg);
		if(result != SQLITE_OK)
		{
			LogError(fmt::format("Could not create table confirm_frame: {}", errorMsg));
			sqlite3_free(errorMsg);
			return;
		}

		result = sqlite3_exec(db_, createConfirmFrameTable, nullptr, nullptr, &errorMsg);
		if(result != SQLITE_OK)
		{
			LogError(fmt::format("Could not create table confirm_frame: {}", errorMsg));
			sqlite3_free(errorMsg);
			return;
		}
	});
	ScheduleAsyncJob(openJob_.get());
}

void CloseDatabase()
{
	if(db_ == nullptr)
		return;
	if(openJob_->HasStarted() && openJob_->IsDone())
	{
		sqlite3_close(db_);
		db_ = nullptr;
	}
}

void AddConfirmFrame(const Checksum<7>& checksum, int confirmFrame)
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	if(db_ == nullptr)
	{
		return;
	}
	if(confirmFrameJob_ != nullptr)
	{
		if(confirmFrameJob_->HasStarted() && !confirmFrameJob_->IsDone())
		{
			confirmFrameJob_->Join();
		}
	}

	confirmFrameJob_ = std::make_unique<neko::FuncJob>([checksum, confirmFrame](){
#ifdef TRACY_ENABLE
		ZoneNamedN(dbInsert, "Insert Confirm Db", true);
#endif
		const auto checksumStatement = fmt::format("INSERT INTO confirm_frame ("
												   "confirm_frame,"
												   "player_character_checksum,"
												   "player_physics_checksum,"
												   "player_input_checksum,"
												   "player_previous_input_checksum,"
												   "player_bodies_checksum,"
												   "bullets_checksum,"
												   "bullet_bodies_checksum,"
												   "final_checksum)"
												   " VALUES ("
												   "{},{},{},{},{},{},{},{},{});",
												   confirmFrame,
												   checksum[0],
												   checksum[1],
												   checksum[2],
												   checksum[3],
												   checksum[4],
												   checksum[5],
												   checksum[6],
												   (std::uint32_t)checksum);
		char* errorMsg = nullptr;
		const auto result = sqlite3_exec(db_, checksumStatement.data(), nullptr, nullptr, &errorMsg);
		if(result != SQLITE_OK)
		{
			LogError(fmt::format("Could not insert confirm_frame: {}", errorMsg));
			sqlite3_free(errorMsg);
			return;
		}
	});
	ScheduleAsyncJob(confirmFrameJob_.get());
}

void AddLocalInput(int currentFrame, PlayerInput playerInput)
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	if(db_ == nullptr)
	{
		return;
	}
	if(localInputJob_ != nullptr)
	{
		if(localInputJob_->HasStarted() && !localInputJob_->IsDone())
		{
			localInputJob_->Join();
		}
	}
	localInputJob_ = std::make_unique<neko::FuncJob>([currentFrame, playerInput](){
#ifdef TRACY_ENABLE
		ZoneNamedN(dbInsert, "Insert Input Db", true);
#endif
		const auto inputStatment = fmt::format("INSERT INTO local_inputs ("
				"frame ,"
				"move_x,"
				"move_y,"
				"target_x,"
				"target_y,"
				"button) VALUES ({},{},{},{},{},{});",
				currentFrame,
				playerInput.moveDirX.underlyingValue(),
				playerInput.moveDirY.underlyingValue(),
				playerInput.targetDirX.underlyingValue(),
				playerInput.targetDirY.underlyingValue(),
				playerInput.buttons	);
		char* errorMsg = nullptr;
		const auto result = sqlite3_exec(db_, inputStatment.data(), nullptr, nullptr, &errorMsg);
		if(result != SQLITE_OK)
		{
			LogError(fmt::format("Could not insert local input: {}", errorMsg));
			sqlite3_free(errorMsg);
			return;
		}

	});
	ScheduleAsyncJob(localInputJob_.get());

}

}
#endif