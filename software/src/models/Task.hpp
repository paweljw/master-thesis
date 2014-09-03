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
			uchr _Type;
			int _WaveID;
			uchr _State;
			void touch();
			std::string Node;
			std::string _MCX;
			void setState(uchr);
			std::string _Metafile;
			boost::posix_time::ptime Created;
			boost::posix_time::ptime Updated;
			boost::posix_time::ptime Completed;
		public:
			Task();
			Task(int);
			int getID();
			int getWave();
			uchr getType();
			void setID(int);
			uchr getState();
			void setWave(int);
			void markBroken();
			void markCreated();
			void setType(uchr);
			void markCompleted();
			std::string getMCX();
			std::string getNode();
			void setMCX(std::string);
			void setNode(std::string);
			std::string getMetafile();
			void setMetafile(std::string);
			boost::posix_time::ptime getCreated();
			boost::posix_time::ptime getUpdated();
			boost::posix_time::ptime getCompleted();
			boost::posix_time::time_duration getElapsed();
			boost::posix_time::time_duration getDuration();
		};
	}
}