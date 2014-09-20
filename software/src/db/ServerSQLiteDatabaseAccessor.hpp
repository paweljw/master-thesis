#include <string.h>

#include "lib\sqlite\sqlite3.h"

#include "db\ServerDatabaseAccessor.hpp"

#include "models\Solution.hpp"
#include "models\Wave.hpp"
#include "models\Task.hpp"

#include "utilhead.hpp"


#pragma once
using namespace logging::trivial;


namespace horizon
{
	namespace db
	{
		class ServerSQLiteDatabaseAccessor : public horizon::db::ServerDatabaseAccessor
		{
		private:
			sqlite3 *database;
			src::severity_logger< severity_level > lg;
			bool performNonQuery(std::string, std::string);
			sqlite3_int64 lastInsertId();
		public:
			ServerSQLiteDatabaseAccessor(std::string);
			bool RecreateDatabase();
			
			int RegisterSolution(horizon::models::Solution&);
			int UpdateSolution(horizon::models::Solution&);
			void FillSolution(horizon::models::Solution&);

			int RegisterWave(horizon::models::Wave&);
			int UpdateWave(horizon::models::Wave&);
			void FillWave(horizon::models::Wave&);

			int RegisterTask(horizon::models::Task&);
			int UpdateTask(horizon::models::Task&);
			void FillTask(horizon::models::Task&);

			bool BeginTransaction();
			bool CommitTransaction();

			std::vector<horizon::models::Task> TaskList(int);
			std::string TaskListJSON(std::vector<horizon::models::Task>);
			void MassMarkTasksSent(std::vector<horizon::models::Task>);
		};

	};
};