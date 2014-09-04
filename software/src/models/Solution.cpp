#include "Solution.hpp"

namespace horizon
{
	namespace models
	{

		Solution::Solution()
		{
			setID(0);
			setState(SOLUTION_UNPUBLISHED);
			setWave(0);
			setSolution(0);
			this->Created = boost::posix_time::microsec_clock::universal_time();
			touch();
		}

		Solution::Solution(int _ID)
		{
			setID(_ID);
			setState(SOLUTION_UNPUBLISHED);
			setWave(0);
			setSolution(0);
			this->Created = boost::posix_time::microsec_clock::universal_time();
			touch();
		}

		int Solution::getID()
		{
			return this->ID;
		}

		void Solution::setID(int _ID)
		{
			this->ID = _ID;
			touch();
		}

		uchr Solution::getState()
		{
			return this->State;
		}

		void Solution::setState(uchr _State)
		{
			this->State = _State;
			touch();
		}

		int Solution::getWave()
		{
			return this->CurrentWave;
		}

		void Solution::setWave(int _Wave)
		{
			this->CurrentWave = _Wave;
			touch();
		}

		std::string Solution::getSolution()
		{
			return this->_Solution;
		}

		void Solution::setSolution(std::string __Solution)
		{
			this->_Solution = __Solution;
			touch();
		}

		void Solution::touch()
		{
			this->Updated = boost::posix_time::microsec_clock::universal_time();
		}

		void Solution::markStarted()
		{
			setState(SOLUTION_AVAILABLE);
		}

		void Solution::markBroken()
		{
			setState(SOLUTION_BROKEN);
		}

		void Solution::markComplete(std::string _Solution)
		{
			setState(SOLUTION_COMPLETE);
			setSolution(_Solution);
			this->Completed = boost::posix_time::microsec_clock::universal_time();
		}

		boost::posix_time::ptime Solution::getCreated()
		{
			return this->Created;
		}

		boost::posix_time::ptime Solution::getUpdated()
		{
			return this->Updated;
		}

		boost::posix_time::ptime Solution::getCompleted()
		{
			return this->Completed;
		}

		boost::posix_time::time_duration Solution::getDuration()
		{
			return (this->Completed - this->Created);
		}

		boost::posix_time::time_duration Solution::getElapsed()
		{
			return (boost::posix_time::microsec_clock::universal_time() - this->Created);
		}

		std::string Solution::getName()
		{
			return this->_Name;
		}

		void Solution::setName(std::string Name)
		{
			this->_Name = Name;
		}
	}
}