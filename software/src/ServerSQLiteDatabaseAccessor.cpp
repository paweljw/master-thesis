#include "lib\sqlite\sqlite3.h"
#include <string.h>

#include "ServerSQLiteDBScheme.hpp"
#include "DatabaseAccessor.hpp"
#include "ServerSQLiteDatabaseAccessor.hpp"

#include "models\Solution.hpp"
#include "models\Wave.hpp"
#include "models\Task.hpp"

#include <boost\filesystem.hpp>

namespace horizon
{
	namespace db
	{
		bool ServerSQLiteDatabaseAccessor::performQuery(std::string query, std::string unitName)
		{

			sqlite3_stmt *statement;

			int prepare_code = sqlite3_prepare(
				this->database,
				query.c_str(),
				-1,
				&statement,
				NULL);

			if (statement != NULL) // this is how it reports errors, imagine that
			{
				int ret = sqlite3_step(statement);
				if (ret == SQLITE_DONE)
					BOOST_LOG_SEV(lg, info) << "Query " << unitName << " successful";
				else
				{
					BOOST_LOG_SEV(lg, warning) << "Query " << unitName << " errored with code " << ret;
					return false;
				}
			}
			else {
				BOOST_LOG_SEV(lg, fatal) << "Query " << unitName << " reported broken at prepare, code " << prepare_code;
				return false;
			}

			sqlite3_finalize(statement);
			return true;
		}

		ServerSQLiteDatabaseAccessor::ServerSQLiteDatabaseAccessor(std::string cs)
		{
			this->ConnectionString = cs;
			sqlite3_open(this->ConnectionString.c_str(), &(this->database));
		}

		bool ServerSQLiteDatabaseAccessor::RecreateDatabase()
		{
			// remove file
			if(sqlite3_close(this->database))
			{
				BOOST_LOG_SEV(lg, warning) << "Failed to close DB, it's probably corrupted now";
				return false;
			}

			boost::filesystem::remove(this->ConnectionString);

			if(sqlite3_open(this->ConnectionString.c_str(), &(this->database)))
			{
				BOOST_LOG_SEV(lg, warning) << "Failed to reopen DB, something's definitely wrong";
				return false;
			}

			if (!performQuery(horizon::dbscheme::simulations, "Simulations")) return false;
			if (!performQuery(horizon::dbscheme::waves, "Waves")) return false;
			if (!performQuery(horizon::dbscheme::tasks, "Tasks")) return false;

			BOOST_LOG_SEV(lg, info) << "Server database recreated successfully";

			return true;
		}

		int ServerSQLiteDatabaseAccessor::RegisterSolution(horizon::models::Solution& s)
		{
			// dummy
			return 0;
		}

		int ServerSQLiteDatabaseAccessor::RegisterWave(horizon::models::Wave& w)
		{
			return 0;
		}

		int ServerSQLiteDatabaseAccessor::RegisterTask(horizon::models::Task& t)
		{
			return 0;
		}
	}
}