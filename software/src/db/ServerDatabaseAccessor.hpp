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

		class ServerDatabaseAccessor
		{
		protected:
			std::string ConnectionString;

		public:
			virtual ~ServerDatabaseAccessor() {}
			virtual bool RecreateDatabase() = 0;
			virtual bool BeginTransaction() = 0;
			virtual bool CommitTransaction() = 0;

			virtual int RegisterSolution(horizon::models::Solution&) = 0;
			virtual int UpdateSolution(horizon::models::Solution&) = 0;
			virtual void FillSolution(horizon::models::Solution&) = 0; 

			virtual int RegisterWave(horizon::models::Wave&) = 0;
			virtual int UpdateWave(horizon::models::Wave&) = 0;
			virtual void FillWave(horizon::models::Wave&) = 0; 

			virtual int RegisterTask(horizon::models::Task&) = 0;
			virtual int UpdateTask(horizon::models::Task&) = 0;
			virtual void FillTask(horizon::models::Task&) = 0;

			virtual std::vector<horizon::models::Task> TaskList(int) = 0;
			virtual std::string TaskListJSON(std::vector<horizon::models::Task>) = 0;
			virtual void MassMarkTasksSent(std::vector<horizon::models::Task>) = 0;
		};
	}
}