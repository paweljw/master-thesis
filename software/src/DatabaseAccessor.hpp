#pragma once

#include <string.h>

// SQLITE3 BINDINGS
#include <sqlite3.h>

// UTILITIES
#include "utilhead.hpp"

#include "models/Solution.hpp"
#include "models/Wave.hpp"
#include "models/Task.hpp"

namespace horizon
{
	namespace db
	{
		class DatabaseAccessor
		{
		protected:
			std::string ConnectionString;

		public:
			virtual ~DatabaseAccessor() {}
			virtual bool CreateDatabase() = 0;
			virtual int RegisterSolution(horizon::models::Solution&) = 0;
			virtual int RegisterWave(horizon::models::Wave&) = 0;
			virtual int RegisterTask(horizon::models::Task&) = 0;
		};
	}
}