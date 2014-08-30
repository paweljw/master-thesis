#include <sqlite3.h>
#include <string.h>

#include "SQLiteDBScheme.hpp"
#include "DatabaseAccessor.hpp"
#include "SQLiteDatabaseAccessor.hpp"

#include "models\Solution.hpp"
#include "models\Wave.hpp"
#include "models\Task.hpp"

#include <boost/filesystem.hpp>

namespace horizon
{
	namespace db
	{
		SQLiteDatabaseAccessor::SQLiteDatabaseAccessor(std::string cs)
		{
			this->ConnectionString = cs;
			sqlite3_open(this->ConnectionString.c_str(), &(this->database));
		}

		bool SQLiteDatabaseAccessor::CreateDatabase()
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

			sqlite3_stmt *statement;

			// I now thoroughly despise SQLite bindings.
			int prepare_code = sqlite3_prepare(
				this->database,
				horizon::dbscheme::simulations,
				-1,
				&statement,
				NULL);

			if(statement != NULL) // this is how it reports errors, imagine that
			{
				int ret = sqlite3_step(statement);
				if(ret == SQLITE_DONE)
					BOOST_LOG_SEV(lg, info) << "Simulations created";
				else
				{
					BOOST_LOG_SEV(lg, warning) << "Something is wrong with simulations creation, code " << ret;
					return false;
				}
			} else {
				BOOST_LOG_SEV(lg, fatal) << "Broken simulations statement, prepare returned " << prepare_code;
				return false;
			}

			sqlite3_finalize(statement);

			prepare_code = sqlite3_prepare(
				this->database,
				horizon::dbscheme::waves,
				-1,
				&statement,
				NULL);

			if(statement != NULL) // this is how it reports errors, imagine that
			{
				int ret = sqlite3_step(statement);
				if(ret == SQLITE_DONE)
					BOOST_LOG_SEV(lg, info) << "Waves created";
				else
				{
					BOOST_LOG_SEV(lg, warning) << "Something is wrong with waves creation, code " << ret;
					sqlite3_finalize(statement);
					return false;
				}
			} else {
				BOOST_LOG_SEV(lg, fatal) << "Broken waves statement, prepare returned " << prepare_code;
				sqlite3_finalize(statement);
				return false;
			}

			sqlite3_finalize(statement);

			prepare_code = sqlite3_prepare(
				this->database,
				horizon::dbscheme::tasks,
				-1,
				&statement,
				NULL);

			if(statement != NULL) // this is how it reports errors, imagine that
			{
				int ret = sqlite3_step(statement);
				if(ret == SQLITE_DONE)
					BOOST_LOG_SEV(lg, info) << "Tasks created";
				else
				{
					BOOST_LOG_SEV(lg, warning) << "Something is wrong with tasks creation, code " << ret;
					sqlite3_finalize(statement);
					return false;
				}
			} else {
				BOOST_LOG_SEV(lg, fatal) << "Broken tasks statement, prepare returned " << prepare_code;
				sqlite3_finalize(statement);
				return false;
			}

			sqlite3_finalize(statement);

			BOOST_LOG_SEV(lg, info) << "Database recreated successfully";

			return true;
		}

		int SQLiteDatabaseAccessor::RegisterSolution(horizon::models::Solution& s)
		{
			// dummy
			return 0;
		}

		int SQLiteDatabaseAccessor::RegisterWave(horizon::models::Wave& w)
		{
			return 0;
		}

		int SQLiteDatabaseAccessor::RegisterTask(horizon::models::Task& t)
		{
			return 0;
		}
	}
}