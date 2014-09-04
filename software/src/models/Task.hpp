#include <string>
#include "globals.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "boost/algorithm/string/trim.hpp"
#include "utilhead.hpp"
using namespace logging::trivial;

#pragma once

namespace horizon
{
	namespace models
	{
		// Task types
		const int TASK_UNDEFINED = 0;
		const int TASK_PART_SOLUTION = 1;
		const int TASK_PART_REDUCE = 2;

		// Task states
		const int TASK_NOT_READY = 0;
		const int TASK_READY = 1;
		const int TASK_SENT = 2;
		const int TASK_RECEIVED = 3;
		const int TASK_PROVISIONED = 4;
		const int TASK_STARTED = 5;
		const int TASK_PROCESSED = 6;
		const int TASK_COMPLETE = 7;
		const int TASK_BROKEN = 9;

		class Task
		{
		private:
			src::severity_logger< severity_level > lg;
			int _ID; //
			uchr _Type; //
			int _WaveID; //
			uchr _State; //
			void touch();
			std::string _Node; //
			int _PartNum; //
			std::string _Name; //
			std::string _Metafile; //
			boost::posix_time::ptime Created;
			boost::posix_time::ptime Updated;
			boost::posix_time::ptime Completed;
		public:
			void setState(uchr);
			Task();
			Task(int);
			int getID();
			int getWave();
			uchr getType();
			void setID(int);
			uchr getState();
			void setWave(int);
			void setType(uchr);
			std::string getNode();
			void setNode(std::string);
			std::string getMetafile();
			void setMetafile(std::string);
			boost::posix_time::ptime getCreated();
			boost::posix_time::ptime getUpdated();
			boost::posix_time::ptime getCompleted();
			boost::posix_time::time_duration getElapsed();
			boost::posix_time::time_duration getDuration();
			int getPartNum();
			void setPartNum(int);
			std::string getName();
			void setName(std::string);
			void markReady();
			void markSent();
			void markReceived();
			void markProvisioned();
			void markStarted();
			void markProcessed();
			void markBroken();
			void markCreated();
			void markCompleted();
			void setCreated(std::string);
			void setUpdated(std::string);
			void setCompleted(std::string);
		};
	}
}