#include "lib/sqlite/sqlite3.h"
#include <string.h>

#include "db/ServerSQLiteDBScheme.hpp"
#include "db/ServerDatabaseAccessor.hpp"
#include "db/ServerSQLiteDatabaseAccessor.hpp"

#include "models\Solution.hpp"
#include "models\Wave.hpp"
#include "models\Task.hpp"

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include "lib/json_spirit/json_spirit.h"

namespace horizon
{
	namespace db
	{
		std::string safe_reinterpret_cast(const unsigned char * column)
		{
			if(column==0) 
				return "";

			return std::string(reinterpret_cast<const char*>(column));
		}

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

			try
			{
				boost::filesystem::remove(this->ConnectionString);
			} catch(boost::filesystem::filesystem_error e) {
				BOOST_LOG_SEV(lg, warning) << "Filesystem error: " << e.what() << " - call aborted";
				return false;
			}

			if(sqlite3_open_v2(this->ConnectionString.c_str(), &(this->database), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL))
			{
				BOOST_LOG_SEV(lg, warning) << "Failed to reopen DB, something's definitely wrong";
				return false;
			}

			HORIZON_UNLESS(performNonQuery(horizon::dbscheme::simulations, "Simulations")) return false;
			HORIZON_UNLESS(performNonQuery(horizon::dbscheme::waves, "Waves")) return false;
			HORIZON_UNLESS(performNonQuery(horizon::dbscheme::tasks, "Tasks")) return false;

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

			boost::algorithm::replace_first(sql, "@created", horizon::sqlite3_time(s.getCreated()));
			boost::algorithm::replace_first(sql, "@updated", horizon::sqlite3_time(s.getUpdated()));
			boost::algorithm::replace_first(sql, "@completed", horizon::sqlite3_time(s.getCompleted()));

			//BOOST_LOG_SEV(lg, info) << "SQL string is now " << sql;

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
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterSolution: code " << bind_code << " at parameter 1";

			bind_code = sqlite3_bind_int(statement, 2, s.getWave());
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterSolution: code " << bind_code << " at parameter 2";

			bind_code = sqlite3_bind_text(statement, 3, s.getName().c_str(), -1, SQLITE_TRANSIENT); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterSolution: code " << bind_code << " at parameter 3";

			bind_code = sqlite3_bind_text(statement, 4, s.getSolution().c_str(), -1, SQLITE_TRANSIENT); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterSolution: code " << bind_code << " at parameter 4";

			bind_code = sqlite3_step(statement);
			HORIZON_UNLESS(bind_code == SQLITE_DONE) BOOST_LOG_SEV(lg, warning) << "RegisterSolution: code " << bind_code << " at sqlite3_step";

			bind_code = sqlite3_finalize(statement);
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterSolution: code " << bind_code << " at sqlite3_finalize";

			this->CommitTransaction();

			s.setID(static_cast<int>(this->lastInsertId()));

			return s.getID();
		}

		int ServerSQLiteDatabaseAccessor::UpdateSolution(horizon::models::Solution& s)
		{
			// begin our transaction
			this->BeginTransaction();

			// prepare sql text
			std::string sql =	"UPDATE solutions SET state=?, current_wave=?, name=?, solution=?, created=@created, updated=@updated, completed=@completed "
								"WHERE id=?;";

			boost::algorithm::replace_first(sql, "@created", horizon::sqlite3_time(s.getCreated()));
			boost::algorithm::replace_first(sql, "@updated", horizon::sqlite3_time(s.getUpdated()));
			boost::algorithm::replace_first(sql, "@completed", horizon::sqlite3_time(s.getCompleted()));

			// prepare and fill statement object
			sqlite3_stmt *statement;
			int prepare_code = sqlite3_prepare_v2(this->database, sql.c_str(), -1, &statement, NULL);

			HORIZON_UNLESS(prepare_code == SQLITE_OK)
				BOOST_LOG_SEV(lg, warning) << "UpdateSolution: prepare code wrong, " << prepare_code;

			if(statement == NULL)
			{
				BOOST_LOG_SEV(lg, fatal) << "UpdateSolution: statement is null";
				return 0;
			}

			// bind data to params
			int bind_code;
			
			bind_code = sqlite3_bind_int(statement, 1, s.getState());
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateSolution: code " << bind_code << " at parameter 1";

			bind_code = sqlite3_bind_int(statement, 2, s.getWave());
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateSolution: code " << bind_code << " at parameter 2";

			bind_code = sqlite3_bind_text(statement, 3, s.getName().c_str(), -1, SQLITE_TRANSIENT); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateSolution: code " << bind_code << " at parameter 3";

			bind_code = sqlite3_bind_text(statement, 4, s.getSolution().c_str(), -1, SQLITE_TRANSIENT); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateSolution: code " << bind_code << " at parameter 4";

			bind_code = sqlite3_bind_int(statement, 5, s.getID());
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateSolution: code " << bind_code << " at parameter 5";

			bind_code = sqlite3_step(statement);
			HORIZON_UNLESS(bind_code == SQLITE_DONE) BOOST_LOG_SEV(lg, warning) << "UpdateSolution: code " << bind_code << " at sqlite3_step";

			bind_code = sqlite3_finalize(statement);
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateSolution: code " << bind_code << " at sqlite3_finalize";

			this->CommitTransaction();

			return s.getID();
		}
		
		void ServerSQLiteDatabaseAccessor::FillSolution(horizon::models::Solution& s)
		{
			if(s.getID() < 1)
				return;

			std::string sql =	"SELECT state, current_wave, name, solution, "
								"strftime('%Y-%m-%d %H:%M:%f', created) AS created, "
								"strftime('%Y-%m-%d %H:%M:%f', updated) AS updated, "
								"strftime('%Y-%m-%d %H:%M:%f', completed) AS completed "
								"FROM solutions WHERE id = ?;";

			sqlite3_stmt *statement;
			int prepare_code = sqlite3_prepare_v2(this->database, sql.c_str(), -1, &statement, NULL);

			HORIZON_UNLESS(prepare_code == SQLITE_OK)
				BOOST_LOG_SEV(lg, warning) << "FillSolution: prepare code wrong, " << prepare_code;

			if(statement == NULL)
			{
				BOOST_LOG_SEV(lg, fatal) << "FillSolution: statement is null";
				return;
			}

			// bind data to params
			int bind_code;
			
			bind_code = sqlite3_bind_int(statement, 1, s.getID());
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "FillSolution: code " << bind_code << " at parameter 1";

			sqlite3_step(statement);

			s.setState(sqlite3_column_int(statement, 0));
			s.setWave(sqlite3_column_int(statement, 1));
			s.setName(std::string(safe_reinterpret_cast(sqlite3_column_text(statement, 2))));
			s.setSolution(std::string(safe_reinterpret_cast(sqlite3_column_text(statement, 3))));
			s.setCreated(std::string(safe_reinterpret_cast(sqlite3_column_text(statement, 4))));
			s.setUpdated(std::string(safe_reinterpret_cast(sqlite3_column_text(statement, 5))));
			s.setCompleted(std::string(safe_reinterpret_cast(sqlite3_column_text(statement, 6))));

			sqlite3_finalize(statement);
		}

		int ServerSQLiteDatabaseAccessor::RegisterWave(horizon::models::Wave& w)
		{
			// begin our transaction
			this->BeginTransaction();

			// prepare sql text
			std::string sql =	"INSERT INTO waves (solution_id, seq, tasks, state, created, updated, completed) "
								"VALUES (?, ?, ?, ?, @created, @updated, @completed);";

			boost::algorithm::replace_first(sql, "@created", horizon::sqlite3_time(w.getCreated()));
			boost::algorithm::replace_first(sql, "@updated", horizon::sqlite3_time(w.getUpdated()));
			boost::algorithm::replace_first(sql, "@completed", horizon::sqlite3_time(w.getCompleted()));

			//BOOST_LOG_SEV(lg, info) << "SQL string is now " << sql;

			// prepare and fill statement object
			sqlite3_stmt *statement;
			int prepare_code = sqlite3_prepare_v2(this->database, sql.c_str(), -1, &statement, NULL);

			if(prepare_code != SQLITE_OK)
				BOOST_LOG_SEV(lg, warning) << "RegisterWave: prepare code wrong, " << prepare_code;

			if(statement == NULL)
			{
				BOOST_LOG_SEV(lg, fatal) << "RegisterWave: statement is null";
				return 0;
			}

			// bind data to params
			int bind_code;
			
			bind_code = sqlite3_bind_int(statement, 1, w.getSolutionID());
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterWave: code " << bind_code << " at parameter 1";

			bind_code = sqlite3_bind_int(statement, 2, w.getSeq());
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterWave: code " << bind_code << " at parameter 2";

			bind_code = sqlite3_bind_int(statement, 3, w.getTasks()); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterWave: code " << bind_code << " at parameter 3";

			bind_code = sqlite3_bind_int(statement, 4, w.getState()); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterWave: code " << bind_code << " at parameter 4";

			bind_code = sqlite3_step(statement);
			HORIZON_UNLESS(bind_code == SQLITE_DONE) BOOST_LOG_SEV(lg, warning) << "RegisterWave: code " << bind_code << " at sqlite3_step";

			bind_code = sqlite3_finalize(statement);
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterWave: code " << bind_code << " at sqlite3_finalize";

			this->CommitTransaction();

			w.setID(static_cast<int>(this->lastInsertId()));

			return w.getID();
		}

		int ServerSQLiteDatabaseAccessor::UpdateWave(horizon::models::Wave& w)
		{
			// begin our transaction
			this->BeginTransaction();

			// prepare sql text
			std::string sql =	"UPDATE waves SET solution_id=?, seq=?, tasks=?, state=?, created=@created, updated=@updated, completed=@completed "
								"WHERE id=?;";
			
			boost::algorithm::replace_first(sql, "@created", horizon::sqlite3_time(w.getCreated()));
			boost::algorithm::replace_first(sql, "@updated", horizon::sqlite3_time(w.getUpdated()));
			boost::algorithm::replace_first(sql, "@completed", horizon::sqlite3_time(w.getCompleted()));

			//BOOST_LOG_SEV(lg, info) << "SQL string is now " << sql;

			// prepare and fill statement object
			sqlite3_stmt *statement;
			int prepare_code = sqlite3_prepare_v2(this->database, sql.c_str(), -1, &statement, NULL);

			if(prepare_code != SQLITE_OK)
				BOOST_LOG_SEV(lg, warning) << "UpdateWave: prepare code wrong, " << prepare_code;

			if(statement == NULL)
			{
				BOOST_LOG_SEV(lg, fatal) << "UpdateWave: statement is null";
				return 0;
			}

			// bind data to params
			int bind_code;
			
			bind_code = sqlite3_bind_int(statement, 1, w.getSolutionID());
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateWave: code " << bind_code << " at parameter 1";

			bind_code = sqlite3_bind_int(statement, 2, w.getSeq());
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateWave: code " << bind_code << " at parameter 2";

			bind_code = sqlite3_bind_int(statement, 3, w.getTasks()); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateWave: code " << bind_code << " at parameter 3";

			bind_code = sqlite3_bind_int(statement, 4, w.getState()); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateWave: code " << bind_code << " at parameter 4";

			bind_code = sqlite3_bind_int(statement, 5, w.getID());
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateWave: code " << bind_code << " at parameter 5";

			bind_code = sqlite3_step(statement);
			HORIZON_UNLESS(bind_code == SQLITE_DONE) BOOST_LOG_SEV(lg, warning) << "UpdateWave: code " << bind_code << " at sqlite3_step";

			bind_code = sqlite3_finalize(statement);
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateWave: code " << bind_code << " at sqlite3_finalize";

			this->CommitTransaction();

			return w.getID();
		}
		
		void ServerSQLiteDatabaseAccessor::FillWave(horizon::models::Wave& w)
		{
			
			if(w.getID() < 1)
				return;

			std::string sql =	"SELECT solution_id, seq, tasks, state, "
								"strftime('%Y-%m-%d %H:%M:%f', created) AS created, "
								"strftime('%Y-%m-%d %H:%M:%f', updated) AS updated, "
								"strftime('%Y-%m-%d %H:%M:%f', completed) AS completed "
								"FROM waves WHERE id = ?;";

			sqlite3_stmt *statement;
			int prepare_code = sqlite3_prepare_v2(this->database, sql.c_str(), -1, &statement, NULL);

			HORIZON_UNLESS(prepare_code == SQLITE_OK)
				BOOST_LOG_SEV(lg, warning) << "FillWave: prepare code wrong, " << prepare_code;

			if(statement == NULL)
			{
				BOOST_LOG_SEV(lg, fatal) << "FillWave: statement is null";
				return;
			}

			// bind data to params
			int bind_code;
			
			bind_code = sqlite3_bind_int(statement, 1, w.getID());
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "FillWave: code " << bind_code << " at parameter 1";

			sqlite3_step(statement);

			w.setSolutionID(sqlite3_column_int(statement, 0));
			w.setSeq(sqlite3_column_int(statement, 1));
			w.setTasks(sqlite3_column_int(statement, 2));
			w.setState(sqlite3_column_int(statement, 3));
			w.setCreated(safe_reinterpret_cast(sqlite3_column_text(statement, 4)));
			w.setUpdated(safe_reinterpret_cast(sqlite3_column_text(statement, 5)));
			w.setCompleted(safe_reinterpret_cast(sqlite3_column_text(statement, 6)));

			sqlite3_finalize(statement);
		}

		int ServerSQLiteDatabaseAccessor::RegisterTask(horizon::models::Task& t)
		{
			// begin our transaction
			this->BeginTransaction();

			// prepare sql text
			std::string sql =	"INSERT INTO tasks (wave_id, type, state, part_num, metafile, node, name, created, updated, completed) "
								"VALUES (?, ?, ?, ?, ?, ?, ?, @created, @updated, @completed);";

			boost::algorithm::replace_first(sql, "@created", horizon::sqlite3_time(t.getCreated()));
			boost::algorithm::replace_first(sql, "@updated", horizon::sqlite3_time(t.getUpdated()));
			boost::algorithm::replace_first(sql, "@completed", horizon::sqlite3_time(t.getCompleted()));

			//BOOST_LOG_SEV(lg, info) << "SQL string is now " << sql;

			// prepare and fill statement object
			sqlite3_stmt *statement;
			int prepare_code = sqlite3_prepare_v2(this->database, sql.c_str(), -1, &statement, NULL);

			if(prepare_code != SQLITE_OK)
				BOOST_LOG_SEV(lg, warning) << "RegisterTask: prepare code wrong, " << prepare_code;

			if(statement == NULL)
			{
				BOOST_LOG_SEV(lg, fatal) << "RegisterTask: statement is null";
				return 0;
			}

			// bind data to params
			int bind_code;

			bind_code = sqlite3_bind_int(statement, 1, t.getWave());
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterTask: code " << bind_code << " at parameter 1";

			bind_code = sqlite3_bind_int(statement, 2, t.getType());
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterTask: code " << bind_code << " at parameter 2";

			bind_code = sqlite3_bind_int(statement, 3, t.getState()); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterTask: code " << bind_code << " at parameter 3";

			bind_code = sqlite3_bind_int(statement, 4, t.getPartNum()); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterTask: code " << bind_code << " at parameter 4";

			bind_code = sqlite3_bind_text(statement, 5, t.getMetafile().c_str(), -1, SQLITE_TRANSIENT); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterTask: code " << bind_code << " at parameter 5";

			bind_code = sqlite3_bind_text(statement, 6, t.getNode().c_str(), -1, SQLITE_TRANSIENT); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterTask: code " << bind_code << " at parameter 6";

			bind_code = sqlite3_bind_text(statement, 7, t.getName().c_str(), -1, SQLITE_TRANSIENT); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterTask: code " << bind_code << " at parameter 7";

			bind_code = sqlite3_step(statement);
			HORIZON_UNLESS(bind_code == SQLITE_DONE) BOOST_LOG_SEV(lg, warning) << "RegisterTask: code " << bind_code << " at sqlite3_step";

			bind_code = sqlite3_finalize(statement);
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "RegisterTask: code " << bind_code << " at sqlite3_finalize";

			this->CommitTransaction();

			t.setID(static_cast<int>(this->lastInsertId()));

			return t.getID();
		}

		int ServerSQLiteDatabaseAccessor::UpdateTask(horizon::models::Task& t)
		{
			// begin our transaction
			this->BeginTransaction();

			// prepare sql text
			std::string sql =	"UPDATE tasks SET wave_id=?, type=?, state=?, part_num=?, metafile=?, node=?, name=?, "
								"created=@created, updated=@updated, completed=@completed "
								"WHERE id=?;";

			boost::algorithm::replace_first(sql, "@created", horizon::sqlite3_time(t.getCreated()));
			boost::algorithm::replace_first(sql, "@updated", horizon::sqlite3_time(t.getUpdated()));
			boost::algorithm::replace_first(sql, "@completed", horizon::sqlite3_time(t.getCompleted()));

			// prepare and fill statement object
			sqlite3_stmt *statement;
			int prepare_code = sqlite3_prepare_v2(this->database, sql.c_str(), -1, &statement, NULL);

			if(prepare_code != SQLITE_OK)
				BOOST_LOG_SEV(lg, warning) << "UpdateTask: prepare code wrong, " << prepare_code;

			if(statement == NULL)
			{
				BOOST_LOG_SEV(lg, fatal) << "UpdateTask: statement is null";
				return 0;
			}

			// bind data to params
			int bind_code;

			bind_code = sqlite3_bind_int(statement, 1, t.getWave());
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateTask: code " << bind_code << " at parameter 1";

			bind_code = sqlite3_bind_int(statement, 2, t.getType());
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateTask: code " << bind_code << " at parameter 2";

			bind_code = sqlite3_bind_int(statement, 3, t.getState()); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateTask: code " << bind_code << " at parameter 3";

			bind_code = sqlite3_bind_int(statement, 4, t.getPartNum()); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateTask: code " << bind_code << " at parameter 4";

			bind_code = sqlite3_bind_text(statement, 5, t.getMetafile().c_str(), -1, SQLITE_TRANSIENT); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateTask: code " << bind_code << " at parameter 5";

			bind_code = sqlite3_bind_text(statement, 6, t.getNode().c_str(), -1, SQLITE_TRANSIENT); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateTask: code " << bind_code << " at parameter 6";

			bind_code = sqlite3_bind_text(statement, 7, t.getName().c_str(), -1, SQLITE_TRANSIENT); // not sure if SQLITE_STATIC would work
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateTask: code " << bind_code << " at parameter 7";
			
			bind_code = sqlite3_bind_int(statement, 8, t.getID());
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateTask: code " << bind_code << " at parameter 8";

			bind_code = sqlite3_step(statement);
			HORIZON_UNLESS(bind_code == SQLITE_DONE) BOOST_LOG_SEV(lg, warning) << "UpdateTask: code " << bind_code << " at sqlite3_step";

			bind_code = sqlite3_finalize(statement);
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "UpdateTask: code " << bind_code << " at sqlite3_finalize";

			this->CommitTransaction();

			return t.getID();
		}
		
		void ServerSQLiteDatabaseAccessor::FillTask(horizon::models::Task& t)
		{
			
			if(t.getID() < 1)
				throw std::exception("ServerSQLiteDatabaseAccessor::FillTask: bad ID");

			std::string sql =	"SELECT wave_id, type, state, part_num, metafile, node, name, "
								"strftime('%Y-%m-%d %H:%M:%f', created) AS created, "
								"strftime('%Y-%m-%d %H:%M:%f', updated) AS updated, "
								"strftime('%Y-%m-%d %H:%M:%f', completed) AS completed "
								"FROM tasks WHERE id = ?;";

			sqlite3_stmt *statement;
			int prepare_code = sqlite3_prepare_v2(this->database, sql.c_str(), -1, &statement, NULL);

			HORIZON_UNLESS(prepare_code == SQLITE_OK)
				BOOST_LOG_SEV(lg, warning) << "FillTask: prepare code wrong, " << prepare_code;

			if(statement == NULL)
			{
				BOOST_LOG_SEV(lg, fatal) << "FillTask: statement is null";
				sqlite3_finalize(statement);
				throw std::exception("ServerSQLiteDatabaseAccessor::FillTask: bad statement");
			}

			// bind data to params
			int bind_code;
			
			bind_code = sqlite3_bind_int(statement, 1, t.getID());
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "FillTask: code " << bind_code << " at parameter 1";

			HORIZON_UNLESS(sqlite3_step(statement) == SQLITE_ROW)
			{
				BOOST_LOG_SEV(lg, fatal) << "FillTask: no data";
				sqlite3_finalize(statement);
				throw std::exception("ServerSQLiteDatabaseAccessor::FillTask: no data");
			}

			t.setWave(sqlite3_column_int(statement, 0));
			t.setType(sqlite3_column_int(statement, 1));
			t.setState(sqlite3_column_int(statement, 2));
			t.setPartNum(sqlite3_column_int(statement, 3));
			t.setMetafile(safe_reinterpret_cast(sqlite3_column_text(statement, 4)));
			t.setNode(safe_reinterpret_cast(sqlite3_column_text(statement, 5)));
			t.setName(safe_reinterpret_cast(sqlite3_column_text(statement, 6)));
			t.setCreated(safe_reinterpret_cast(sqlite3_column_text(statement, 7)));
			t.setUpdated(safe_reinterpret_cast(sqlite3_column_text(statement, 8)));
			t.setCompleted(safe_reinterpret_cast(sqlite3_column_text(statement, 9)));

			sqlite3_finalize(statement);
		}

		bool ServerSQLiteDatabaseAccessor::BeginTransaction()
		{
			return performNonQuery("BEGIN TRANSACTION;", "BeginTransaction");
		}

		bool ServerSQLiteDatabaseAccessor::CommitTransaction()
		{
			return performNonQuery("COMMIT;", "CommitTransaction");
		}

		std::vector<horizon::models::Task> ServerSQLiteDatabaseAccessor::TaskList(int num)
		{
			std::string sql = "SELECT tasks.id FROM tasks WHERE state = 1 ORDER BY tasks.created ASC LIMIT ?;";

			sqlite3_stmt *statement;
			int prepare_code = sqlite3_prepare_v2(this->database, sql.c_str(), -1, &statement, NULL);

			HORIZON_UNLESS(prepare_code == SQLITE_OK)
				BOOST_LOG_SEV(lg, warning) << "TaskListJSON: prepare code wrong, " << prepare_code;

			if (statement == NULL)
			{
				BOOST_LOG_SEV(lg, fatal) << "FillTask: statement is null";
				sqlite3_finalize(statement);
				throw std::exception("ServerSQLiteDatabaseAccessor::TaskListJSON: bad statement");
			}

			// bind data to params
			int bind_code;

			bind_code = sqlite3_bind_int(statement, 1, num);
			HORIZON_UNLESS(bind_code == SQLITE_OK) BOOST_LOG_SEV(lg, warning) << "TaskListJSON: code " << bind_code << " at parameter 1";

			std::vector<horizon::models::Task> tasks = std::vector<horizon::models::Task>(0);

			while (sqlite3_step(statement) == SQLITE_ROW)
			{
				horizon::models::Task t(sqlite3_column_int(statement, 0));
				this->FillTask(t);
				tasks.push_back(t);
			}

			sqlite3_finalize(statement);

			return tasks;
		}

		void ServerSQLiteDatabaseAccessor::MassMarkTasksSent(std::vector<horizon::models::Task> tasks)
		{
			BOOST_FOREACH(horizon::models::Task t, tasks)
			{
				t.markSent();
				this->UpdateTask(t);
			}
		}

		std::string TaskListJSON(std::vector<horizon::models::Task> tasks)
		{
			json_spirit::Array ar;

			BOOST_FOREACH(horizon::models::Task t, tasks)
			{
				ar.push_back(t.toJSONObject());
			}

			return json_spirit::write(ar, json_spirit::pretty_print);
		}
	}
}