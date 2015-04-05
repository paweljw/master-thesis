#pragma once

#include <string>

#include "models/Solution.hpp"
#include "models/Wave.hpp"
#include "models/Task.hpp"

namespace horizon
{
	namespace db
	{
		std::string safe_reinterpret_cast(const unsigned char *);

		class DatabaseAccessor
		{
		protected:
			/** @brief	The connection string. */
			std::string ConnectionString;

		public:

			/**
			 * @fn	virtual DatabaseAccessor::~DatabaseAccessor()
			 *
			 * @brief	Destructor.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 */

			virtual ~DatabaseAccessor() {}

			/**
			 * @fn	virtual bool DatabaseAccessor::RecreateDatabase() = 0;
			 *
			 * @brief	Recreates the database.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @return	true if it succeeds, false if it fails.
			 */

			virtual bool RecreateDatabase() = 0;

			/**
			 * @fn	virtual bool DatabaseAccessor::BeginTransaction() = 0;
			 *
			 * @brief	Begins a transaction.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @return	true if it succeeds, false if it fails.
			 */

			virtual bool BeginTransaction() = 0;

			/**
			 * @fn	virtual bool DatabaseAccessor::CommitTransaction() = 0;
			 *
			 * @brief	Commits a transaction.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @return	true if it succeeds, false if it fails.
			 */

			virtual bool CommitTransaction() = 0;

			/**
			 * @fn	virtual int DatabaseAccessor::RegisterSolution(horizon::models::Solution&) = 0;
			 *
			 * @brief	Registers the solution described by parameter1.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param [in,out]	parameter1	Input solution.
			 *
			 * @return	Solution ID.
			 */

			virtual int RegisterSolution(horizon::models::Solution&) = 0;

			/**
			 * @fn	virtual int DatabaseAccessor::UpdateSolution(horizon::models::Solution&) = 0;
			 *
			 * @brief	Updates the solution described by parameter1.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param [in,out]	parameter1	The solution to update.
			 *
			 * @return	Solution ID.
			 */

			virtual int UpdateSolution(horizon::models::Solution&) = 0;

			/**
			 * @fn	virtual void DatabaseAccessor::FillSolution(horizon::models::Solution&) = 0;
			 *
			 * @brief	Fill solution with data from DB based on ID.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param [in,out]	parameter1	Solution.
			 * 					
			 * @return Solution ID.
			 */

			virtual void FillSolution(horizon::models::Solution&) = 0; 

			/**
			 * @fn	virtual int DatabaseAccessor::RegisterWave(horizon::models::Wave&) = 0;
			 *
			 * @brief	Registers the wave described by parameter1.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param [in,out]	parameter1	Wave.
			 *
			 * @return	Wave ID.
			 */

			virtual int RegisterWave(horizon::models::Wave&) = 0;

			/**
			 * @fn	virtual int DatabaseAccessor::UpdateWave(horizon::models::Wave&) = 0;
			 *
			 * @brief	Updates the wave described by parameter1.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param [in,out]	parameter1	Waver.
			 *
			 * @return	Wave ID.
			 */

			virtual int UpdateWave(horizon::models::Wave&) = 0;

			/**
			 * @fn	virtual void DatabaseAccessor::FillWave(horizon::models::Wave&) = 0;
			 *
			 * @brief	Fill wave.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param [in,out]	parameter1	Wave.
			 */

			virtual void FillWave(horizon::models::Wave&) = 0; 

			/**
			 * @fn	virtual int DatabaseAccessor::RegisterTask(horizon::models::Task&) = 0;
			 *
			 * @brief	Registers the task described by parameter1.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param [in,out]	parameter1	Task.
			 *
			 * @return	Task ID.
			 */

			virtual int RegisterTask(horizon::models::Task&) = 0;

			/**
			 * @fn	virtual int DatabaseAccessor::UpdateTask(horizon::models::Task&) = 0;
			 *
			 * @brief	Updates the task described by parameter1.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param [in,out]	parameter1	Task.
			 *
			 * @return	Task ID.
			 */

			virtual int UpdateTask(horizon::models::Task&) = 0;

			/**
			 * @fn	virtual void DatabaseAccessor::FillTask(horizon::models::Task&) = 0;
			 *
			 * @brief	Fill task.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param [in,out]	parameter1	Task.
			 */

			virtual void FillTask(horizon::models::Task&) = 0;

			/**
			 * @fn	virtual std::vector<horizon::models::Task> DatabaseAccessor::TaskList(int) = 0;
			 *
			 * @brief	Pulls down a task list of required length in state TASK_READY.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param	num	Number of tasks to pull down.
			 *
			 * @return	A std::vector&lt;horizon::models::Task&gt;
			 */

			virtual std::vector<horizon::models::Task> TaskList(int) = 0;

			/**
			 * @fn	virtual std::string DatabaseAccessor::TaskListToJSON(std::vector<horizon::models::Task>) = 0;
			 *
			 * @brief	Converts a task list to JSON.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param	tasks	A vector of tasks.
			 *
			 * @return	A formatted JSON string.
			 */

			virtual std::string TaskListToJSON(std::vector<horizon::models::Task>) = 0;

			/**
			 * @fn	virtual void DatabaseAccessor::MassMarkTasksSent(std::vector<horizon::models::Task>) = 0;
			 *
			 * @brief	Mass mark tasks sent in the DB.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param	tasks	List of tasks.
			 */

			virtual void MassMarkTasksSent(std::vector<horizon::models::Task>) = 0;

			/**
			 * @fn	virtual std::vector<horizon::models::Task> DatabaseAccessor::JSONToTaskList(std::string) = 0;
			 *
			 * @brief	JSON to task list conversion.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param	json	A JSON string.
			 *
			 * @return	A std::vector&lt;horizon::models::Task&gt;
			 */

			virtual std::vector<horizon::models::Task> JSONToTaskList(std::string) = 0;
		};
	}
}