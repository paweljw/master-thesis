#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "globals.hpp"
#include "utilhead.hpp"
using namespace logging::trivial;

#pragma once

#ifndef __wave_hpp
#define __wave_hpp

namespace horizon
{
	namespace models
	{
		const uchr WAVE_UNPUBLISHED = 0;
		const uchr WAVE_AVAILABLE = 1;
		const uchr WAVE_COMPLETED = 2;
		const uchr WAVE_BROKEN = 9;
			
		class Wave
		{
		private:
			int _ID;
			int _SolutionID;
			int _Seq;
			int _Tasks;
			uchr _State;

			// Constant after create!!!
			boost::posix_time::ptime Created;

			// Auto-updated on DB writes!!!
			boost::posix_time::ptime Updated;

			// Only updated once!!!
			boost::posix_time::ptime Completed;
			void touch();
			src::severity_logger< severity_level > lg;
		public:
			void setState(uchr);
			Wave();
			Wave(int);
			int getID();
			void setID(int);
			int getSolutionID();
			void setSolutionID(int);
			int getSeq();
			void setSeq(int);
			int getTasks();
			void setTasks(int);
			void incrementTasks();
			void decrementTasks();
			uchr getState();
			void markComplete();
			void markAvailable();
			void markBroken();
			boost::posix_time::ptime getCreated();
			boost::posix_time::ptime getUpdated();
			boost::posix_time::ptime getCompleted();
			boost::posix_time::time_duration getDuration();
			boost::posix_time::time_duration getElapsed();
			void setCreated(std::string);
			void setUpdated(std::string);
			void setCompleted(std::string);
		};
	}
}

#endif