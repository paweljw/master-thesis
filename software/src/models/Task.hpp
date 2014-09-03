#include <string>
#include "globals.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

#pragma once

namespace horizon
{
	namespace models
	{
		class Task
		{
		private:
			int _ID;
			int _WaveID;
			uchr _Type;
			uchr _State;
			std::string _MCX;
			std::string _Metafile;
			std::string Node;
			boost::posix_time::ptime Created;
			boost::posix_time::ptime Updated;
			boost::posix_time::ptime Completed;
			void touch();
			void setState(uchr);
		public:
			Task();
			Task(int);
			int getID();
			void setID(int);
			int getWave();
			void setWave(int);
			uchr getType();
			void setType(uchr);
			uchr getState();
			std::string getMCX();
			void setMCX(std::string);
			std::string getMetafile();
			void setMetafile(std::string);
			std::string getNode();
			void setNode(std::string);
			void markCreated();
			void markCompleted();
			void markBroken();
			boost::posix_time::ptime getCreated();
			boost::posix_time::ptime getUpdated();
			boost::posix_time::ptime getCompleted();
			boost::posix_time::time_duration getDuration();
			boost::posix_time::time_duration getElapsed();
		};
	}
}