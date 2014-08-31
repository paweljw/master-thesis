#include <string.h>

#include "lib\sqlite\sqlite3.h"

#include "DatabaseAccessor.hpp"

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
		class SQLiteDatabaseAccessor : public horizon::db::DatabaseAccessor
		{
		private:
			sqlite3 *database;
			src::severity_logger< severity_level > lg;

		public:
			SQLiteDatabaseAccessor(std::string);
			bool CreateDatabase();
			int RegisterSolution(horizon::models::Solution&);
			int RegisterWave(horizon::models::Wave&);
			int RegisterTask(horizon::models::Task&);
		};

	};
};