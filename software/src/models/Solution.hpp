#include <string>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "globals.hpp"

#pragma once

#ifndef __solution_hpp
#define __solution_hpp

namespace horizon
{
	namespace models
	{
		const uchr SOLUTION_UNPUBLISHED	= 0;
		const uchr SOLUTION_AVAILABLE = 1;
		const uchr SOLUTION_COMPLETE = 2;
		const uchr SOLUTION_BROKEN = 9;

		class Solution
		{
		private:
			int ID;
			uchr State;
			int CurrentWave;
			std::string _Solution;
			std::string _Name;
			void setState(uchr);
			// Constant after create!!!
			boost::posix_time::ptime Created;

			// Auto-updated on DB writes!!!
			boost::posix_time::ptime Updated;

			// Only updated once!!!
			boost::posix_time::ptime Completed;
			void touch();
		public:
			Solution();
			Solution(int);
			int getID();
			void setID(int);
			uchr getState();
			int getWave();
			void setWave(int);
			std::string getSolution();
			void setSolution(std::string);
			void markStarted();
			void markComplete(std::string);
			void markBroken();
			boost::posix_time::ptime getCreated();
			boost::posix_time::ptime getUpdated();
			boost::posix_time::ptime getCompleted();
			boost::posix_time::time_duration getDuration();
			boost::posix_time::time_duration getElapsed();
			std::string getName();
			void setName(std::string);
		};
	}
}

#endif