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
#include <rigtorp/SPSCQueue.h>

#include <optional>

#include <filesystem>
namespace fs = std::filesystem;

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>
#endif

namespace splash
{

static sqlite3* db_ = nullptr;
static std::unique_ptr<neko::FuncJob> openJob_{};
static std::unique_ptr<neko::FuncJob> runQueriesJob_{};

struct ConfirmFrameData
{
	Checksum<7> checksum{};
	int confirmFrame{};
};

struct RemoteInputData
{
	int currentFrame{};
	int remoteFrame{};
	int playerNumber{};
	PlayerInput playerInput{};
};

struct LocalInputData
{
	int currentFrame{};
	PlayerInput playerInput{};
};

using QueryData = std::variant<ConfirmFrameData, RemoteInputData, LocalInputData>;
static rigtorp::SPSCQueue<QueryData> queries{50};

static void Query(const ConfirmFrameData& data)
{
#ifdef TRACY_ENABLE
	ZoneScoped;
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
												   data.confirmFrame,
												   data.checksum[0],
												   data.checksum[1],
												   data.checksum[2],
												   data.checksum[3],
												   data.checksum[4],
												   data.checksum[5],
												   data.checksum[6],
												   static_cast<std::uint32_t>(data.checksum));
	char* errorMsg = nullptr;
	const auto result = sqlite3_exec(db_, checksumStatement.data(), nullptr, nullptr, &errorMsg);
	if(result != SQLITE_OK)
	{
		LogError(fmt::format("Could not insert confirm_frame: {}", errorMsg));
		sqlite3_free(errorMsg);
	}
}

static void Query(const RemoteInputData& data)
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	const auto inputStatment = fmt::format("INSERT INTO remote_inputs ("
											   "remote_frame,"
											   "local_frame,"
											   "player_number,"
											   "move_x,"
											   "move_y,"
											   "target_x,"
											   "target_y,"
											   "button) VALUES ({},{},{},{},{},{},{},{});",
				data.remoteFrame,
				data.currentFrame,
				data.playerNumber,
				data.playerInput.moveDirX.underlyingValue(),
				data.playerInput.moveDirY.underlyingValue(),
				data.playerInput.targetDirX.underlyingValue(),
				data.playerInput.targetDirY.underlyingValue(),
				data.playerInput.buttons);
	char* errorMsg = nullptr;
	const auto result = sqlite3_exec(db_, inputStatment.data(), nullptr, nullptr, &errorMsg);
	if(result != SQLITE_OK)
	{
		LogError(fmt::format("Could not insert remote input: {}", errorMsg));
		sqlite3_free(errorMsg);
	}
}

static void Query(const LocalInputData& data)
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	const auto inputStatment = fmt::format("INSERT INTO local_inputs ("
				"frame ,"
				"move_x,"
				"move_y,"
				"target_x,"
				"target_y,"
				"button) VALUES ({},{},{},{},{},{});",
				data.currentFrame,
				data.playerInput.moveDirX.underlyingValue(),
				data.playerInput.moveDirY.underlyingValue(),
				data.playerInput.targetDirX.underlyingValue(),
				data.playerInput.targetDirY.underlyingValue(),
				data.playerInput.buttons	);
	char* errorMsg = nullptr;
	const auto result = sqlite3_exec(db_, inputStatment.data(), nullptr, nullptr, &errorMsg);
	if(result != SQLITE_OK)
	{
		LogError(fmt::format("Could not insert local input: {}", errorMsg));
		sqlite3_free(errorMsg);
	}
}

static void RunQuery(const QueryData& queryData)
{
	std::visit([&](auto&& arg)
	{
		using T = std::decay_t<decltype(arg)>;
		Query(arg);
	}, queryData);
}

static void RunQueries()
{
	if(runQueriesJob_->HasStarted() && !runQueriesJob_->IsDone())
	{
		return;
	}
	runQueriesJob_->Reset();
	ScheduleAsyncJob(runQueriesJob_.get());
}

void OpenDatabase(int playerNumber)
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif

	openJob_ = std::make_unique<neko::FuncJob>([playerNumber](){
#ifdef TRACY_ENABLE
		ZoneNamedN(dbOpen, "Create Database", true);
#endif

		int i = 0;
		std::string dbName;
		do
		{
			dbName = fmt::format("splash{}_p{}.db", i, playerNumber);
			i++;
		} while(fs::exists(dbName));
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
		}
	});
	ScheduleAsyncJob(openJob_.get());
	runQueriesJob_ = std::make_unique<neko::FuncJob>([]()
	{
		while(!queries.empty())
		{
			auto query = *queries.front();
			RunQuery(query);
			queries.pop();
		}
	});
}

void CloseDatabase()
{
	if(db_ == nullptr)
		return;
	if(runQueriesJob_->HasStarted() && !runQueriesJob_->IsDone())
	{
		runQueriesJob_->Join();
	}
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


	queries.push(ConfirmFrameData{checksum, confirmFrame});

	RunQueries();
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

	queries.push(LocalInputData{currentFrame, playerInput});

	RunQueries();
}

void AddRemoteInput(int currentFrame, int remoteFrame, int playerNumber, PlayerInput playerInput)
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	if(db_ == nullptr)
	{
		return;
	}

	queries.push(RemoteInputData{currentFrame, remoteFrame, playerNumber, playerInput});

	RunQueries();
}

}
#endif