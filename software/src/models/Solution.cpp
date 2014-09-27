/**
 * @file	C:\Users\PJW\Documents\GitHub\master-thesis\software\src\models\Solution.cpp
 *
 * @brief	Implements the solution class.
 */

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
			setSolution("");
			setName("");
			this->Created = boost::posix_time::microsec_clock::universal_time();
			touch();
		}

		Solution::Solution(int _ID)
		{
			using namespace logging::trivial;
			src::severity_logger< severity_level > lg;
			setID(_ID);
			setState(SOLUTION_UNPUBLISHED);
			setWave(0);
			setSolution("");
			setName("");
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

		void Solution::markAvailable()
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
			Name.erase(std::remove(Name.begin(), Name.end(), '\\'), Name.end());
			Name.erase(std::remove(Name.begin(), Name.end(), '/'), Name.end());
			boost::algorithm::replace_all(Name, ".", "_");
			this->_Name = Name;
		}

		void Solution::setCreated(std::string _Created)
		{
			HORIZON_UNLESS(boost::algorithm::trim_copy(_Created).empty())
			{
				try
				{
					this->Created = boost::posix_time::time_from_string(_Created);
				} catch(std::exception e) {
					BOOST_LOG_SEV(lg, warning) << "Failed to convert time because " << e.what();
				}
			}
		}

		void Solution::setUpdated(std::string _Updated)
		{
			HORIZON_UNLESS(boost::algorithm::trim_copy(_Updated).empty())
			{
				try
				{
					this->Created = boost::posix_time::time_from_string(_Updated);
				} catch(std::exception e) {
					BOOST_LOG_SEV(lg, warning) << "Failed to convert time because " << e.what();
				}
			}
		}

		void Solution::setCompleted(std::string _Completed)
		{
			HORIZON_UNLESS(boost::algorithm::trim_copy(_Completed).empty())
			{
				try
				{
					this->Created = boost::posix_time::time_from_string(_Completed);
				} catch(std::exception e) {
					BOOST_LOG_SEV(lg, warning) << "Failed to convert time because " << e.what();
				}
			}
		}
	}
}