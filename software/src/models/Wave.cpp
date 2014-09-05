#include "models\Wave.hpp"

namespace horizon
{
	namespace models
	{
		void Wave::setState(uchr State)
		{
			this->_State = State;
		}

		void Wave::touch()
		{
			this->Updated = boost::posix_time::microsec_clock::universal_time();
		}

		Wave::Wave()
		{
			setID(0);
			setSolutionID(0);
			setSeq(0);
			setTasks(0);
			setState(WAVE_UNPUBLISHED);

			this->Created = boost::posix_time::microsec_clock::universal_time();
			touch();
		}
		
		Wave::Wave(int _ID)
		{
			setID(_ID);
			setSolutionID(0);
			setSeq(0);
			setTasks(0);
			setState(WAVE_UNPUBLISHED);

			this->Created = boost::posix_time::microsec_clock::universal_time();
			touch();
		}
		
		int Wave::getID()
		{
			return this->_ID;
		}

		void Wave::setID(int ID)
		{
			this->_ID = ID;
			touch();
		}

		int Wave::getSolutionID()
		{
			return this->_SolutionID;
		}
		
		void Wave::setSolutionID(int SolutionID)
		{
			this->_SolutionID = SolutionID;
			touch();
		}

		int Wave::getSeq()
		{
			return this->_Seq;
		}

		void Wave::setSeq(int Seq)
		{
			this->_Seq = Seq;
			touch();
		}

		int Wave::getTasks()
		{
			return this->_Tasks;
		}

		void Wave::setTasks(int Tasks)
		{
			this->_Tasks = Tasks;
			touch();
		}

		void Wave::incrementTasks()
		{
			this->_Tasks++;
			touch();
		}

		void Wave::decrementTasks()
		{
			this->_Tasks--;
			touch();
		}

		uchr Wave::getState()
		{
			return this->_State;
		}

		void Wave::markComplete()
		{
			this->_State = WAVE_COMPLETED;
			this->Completed = boost::posix_time::microsec_clock::universal_time();
			touch();
		}

		
		void Wave::markAvailable()
		{
			this->_State = WAVE_AVAILABLE;
			touch();
		}

		void Wave::markBroken()
		{
			this->markComplete();
			this->_State = WAVE_BROKEN;
			touch();
		}

		boost::posix_time::ptime Wave::getCreated()
		{
			return this->Created;
		}

		boost::posix_time::ptime Wave::getUpdated()
		{
			return this->Updated;
		}

		boost::posix_time::ptime Wave::getCompleted()
		{
			return this->Completed;
		}

		boost::posix_time::time_duration Wave::getDuration()
		{
			return (this->Completed - this->Created);
		}

		boost::posix_time::time_duration Wave::getElapsed()
		{
			return (boost::posix_time::microsec_clock::universal_time() - this->Created);
		}

		void Wave::setCreated(std::string _Created)
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

		void Wave::setUpdated(std::string _Updated)
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

		void Wave::setCompleted(std::string _Completed)
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