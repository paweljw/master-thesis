#include "lib/sqlite/sqlite3.h"
#include <string.h>

#include "ServerSQLiteDBScheme.hpp"
#include "ServerDatabaseAccessor.hpp"
#include "ServerSQLiteDatabaseAccessor.hpp"

#include "models\Solution.hpp"
#include "models\Wave.hpp"
#include "models\Task.hpp"

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>

namespace horizon
{
	namespace db
	{
		bool ServerSQLiteDatabaseAccessor::performNonQuery(std::string query, std::string unitName)
		{
			if(this->database == NULL)
			{
				BOOST_LOG_SEV(lg, fatal) << "PerformNonQuery: Database is null!";
				return false;
			}

			sqlite3_stmt *statement;

			int prepare_code = sqlite3_prepare_v2(
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

		std::string ServerSQLiteDatabaseAccessor::sqlite3_time(boost::posix_time::ptime bpt)
		{
			std::string ret = boost::posix_time::to_iso_extended_string(bpt);

			if(ret == "not-a-date-time") 
				return "datetime('now')";

			boost::algorithm::replace_first(ret, ",", ".");
			ret.resize(23);
			BOOST_LOG_SEV(lg, info) << "datetime('"  << ret << "')";
			
			return "datetime('" + ret + "')";
		}

		sqlite3_int64 ServerSQLiteDatabaseAccessor::lastInsertId()
		{
			sqlite3_int64 ret = sqlite3_last_insert_rowid(this->database);
			if(static_cast<int>(ret) == 0)
				BOOST_LOG_SEV(lg, warning) << "LastInsertId: Database returned zero";

			return ret;
		}

		ServerSQLiteDatabaseAccessor::ServerSQLiteDatabaseAccessor(std::string cs)
		{
			this->ConnectionString = cs;
			if(sqlite3_open_v2(this->ConnectionString.c_str(), &(this->database), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL))
			{
				BOOST_LOG_SEV(lg, warning) << "Failed to open DB, something's definitely wrong";
			}
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

			if(sqlite3_open_v2(this->ConnectionString.c_str(), &(this->database), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL))
			{
				BOOST_LOG_SEV(lg, warning) << "Failed to reopen DB, something's definitely wrong";
				return false;
			}

			if (!performNonQuery(horizon::dbscheme::simulations, "Simulations")) return false;
			if (!performNonQuery(horizon::dbscheme::waves, "Waves")) return false;
			if (!performNonQuery(horizon::dbscheme::tasks, "Tasks")) return false;

			BOOST_LOG_SEV(lg, info) << "Server database recreated successfully";

			return true;
		}

		int ServerSQLiteDatabaseAccessor::RegisterSolution(horizon::models::Solution& s)
		{
			// begin our transaction
			this->BeginTransaction();

			// prepare sql text
			std::string sql =	"INSERT INTO solutions (state, current_wave, name, solution, created, updated, completed) "
								"VALUES (?, ?, ?, ?, @created, @updated, @completed);";

			boost::algorithm::replace_first(sql, "@created", this->sqlite3_time(s.getCreated()));
			boost::algorithm::replace_first(sql, "@updated", this->sqlite3_time(s.getUpdated()));
			boost::algorithm::replace_first(sql, "@completed", this->sqlite3_time(s.getCompleted()));

			BOOST_LOG_SEV(lg, info) << "SQL string is now " << sql;

			// prepare and fill statement object
			sqlite3_stmt *statement;
			int prepare_code = sqlite3_prepare_v2(this->database, sql.c_str(), -1, &statement, NULL);

			if(prepare_code != SQLITE_OK)
				BOOST_LOG_SEV(lg, warning) << "RegisterSolution: prepare code wrong, " << prepare_code;

			if(statement == NULL)
			{
				BOOST_LOG_SEV(lg, fatal) << "RegisterSolution: statement is null";
				return 0;
			}

			// bind data to params
			int bind_code;
			
			bind_code = sqlite3_bind_int(statement, 1, s.getState());
			if(bind_code != SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterSolution: code " << bind_code << " at parameter 1";

			bind_code = sqlite3_bind_int(statement, 2, s.getWave());
			if(bind_code != SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterSolution: code " << bind_code << " at parameter 2";

			bind_code = sqlite3_bind_text(statement, 3, s.getName().c_str(), -1, SQLITE_TRANSIENT); // not sure if SQLITE_STATIC would work
			if(bind_code != SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterSolution: code " << bind_code << " at parameter 3";

			bind_code = sqlite3_bind_text(statement, 4, s.getSolution().c_str(), -1, SQLITE_TRANSIENT); // not sure if SQLITE_STATIC would work
			if(bind_code != SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterSolution: code " << bind_code << " at parameter 4";

			bind_code = sqlite3_step(statement);
			if(bind_code != SQLITE_DONE) BOOST_LOG_SEV(lg, warning) << "RegisterSolution: code " << bind_code << " at sqlite3_step";

			bind_code = sqlite3_finalize(statement);
			if(bind_code != SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterSolution: code " << bind_code << " at sqlite3_finalize";

			this->CommitTransaction();

			s.setID(static_cast<int>(this->lastInsertId()));

			return s.getID();
		}

		int ServerSQLiteDatabaseAccessor::RegisterWave(horizon::models::Wave& w)
		{
			return 0;
		}

		int ServerSQLiteDatabaseAccessor::RegisterTask(horizon::models::Task& t)
		{
			return 0;
		}

		bool ServerSQLiteDatabaseAccessor::BeginTransaction()
		{
			return performNonQuery("BEGIN TRANSACTION;", "BeginTransaction");
		}

		bool ServerSQLiteDatabaseAccessor::CommitTransaction()
		{
			return performNonQuery("COMMIT;", "CommitTransaction");
		}

	}
}