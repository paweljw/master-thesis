#include "boost/date_time/posix_time/posix_time.hpp"
#include "globals.hpp"

#pragma once

#ifndef __wave_hpp
#define __wave_hpp

namespace horizon
{
	namespace models
	{
		const uchr WAVE_NOT_STARTED = 0;
		const uchr WAVE_STARTED = 1;
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
		public:
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
			void setState(uchr);
			void markComplete();
		};
	}
}

#endif