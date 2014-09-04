#pragma once

#include <string.h>

#include "models/Solution.hpp"
#include "models/Wave.hpp"
#include "models/Task.hpp"

namespace horizon
{
	namespace db
	{
		class ServerDatabaseAccessor
		{
		protected:
			std::string ConnectionString;

		public:
			virtual ~ServerDatabaseAccessor() {}
			virtual bool RecreateDatabase() = 0;
			virtual int RegisterSolution(horizon::models::Solution&) = 0;
			virtual int RegisterWave(horizon::models::Wave&) = 0;
			virtual int RegisterTask(horizon::models::Task&) = 0;
		};
	}
}