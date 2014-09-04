#include "models\Task.hpp"

namespace horizon
{
	namespace models
	{
		Task::Task()
		{
			this->setID(0);
			this->setMetafile("");
			this->setName("");
			this->setNode("");
			this->setPartNum(0);
			this->setType(TASK_UNDEFINED);
			this->setWave(0);
			markCreated();
		}

		Task::Task(int ID)
		{
			this->setID(ID);
			this->setMetafile("");
			this->setName("");
			this->setNode("");
			this->setPartNum(0);
			this->setType(TASK_UNDEFINED);
			this->setWave(0);
			markCreated();
		}

		int Task::getID()
		{
			return this->_ID;
		}

		int Task::getWave()
		{
			return this->_WaveID;
		}

		uchr Task::getType()
		{
			return this->_Type;
		}

		void Task::setID(int ID)
		{
			this->_ID = ID;
		}

		uchr Task::getState()
		{
			return this->_State;
		}

		void Task::setWave(int Wave)
		{
			this->_WaveID = Wave;
		}

		void Task::setType(uchr Type)
		{
			this->_Type = Type;
		}

		std::string Task::getNode()
		{
			return this->_Node;
		}

		void Task::setNode(std::string Node)
		{
			this->_Node = Node;
		}

		std::string Task::getMetafile()
		{
			return this->_Metafile;
		}

		void Task::setMetafile(std::string Metafile)
		{
			this->_Metafile = Metafile;
		}

		void Task::touch()
		{
			this->Updated = boost::posix_time::microsec_clock::universal_time();
		}

		void Task::setState(uchr State)
		{
			touch();
			this->_State = State;
		}

		void Task::markReady()
		{
			setState(TASK_READY);
		}

		void Task::markBroken()
		{
			setState(TASK_BROKEN);
		}

		void Task::markCompleted()
		{
			setState(TASK_COMPLETE);
			this->Completed = boost::posix_time::microsec_clock::universal_time();
		}

		void Task::markCreated()
		{
			setState(TASK_NOT_READY);
			this->Created = boost::posix_time::microsec_clock::universal_time();
		}

		void Task::markProcessed()
		{
			setState(TASK_PROCESSED);
		}

		void Task::markProvisioned()
		{
			setState(TASK_PROVISIONED);
		}

		void Task::markReady()
		{
			setState(TASK_READY);
		}

		void Task::markReceived()
		{
			setState(TASK_RECEIVED);
		}

		void Task::markSent()
		{
			setState(TASK_SENT);
		}

		void Task::markStarted()
		{
			setState(TASK_STARTED);
		}

		boost::posix_time::ptime Task::getCreated()
		{
			return this->Created;
		}

		boost::posix_time::ptime Task::getUpdated()
		{
			return this->Updated;
		}

		boost::posix_time::ptime Task::getCompleted()
		{
			return this->Completed;
		}

		boost::posix_time::time_duration Task::getDuration()
		{
			return (this->Completed - this->Created);
		}

		boost::posix_time::time_duration Task::getElapsed()
		{
			return (boost::posix_time::microsec_clock::universal_time() - this->Created);
		}

		int Task::getPartNum()
		{
			return this->_PartNum;
		}

		void Task::setPartNum(int PartNum)
		{
			this->_PartNum = PartNum;
		}

		std::string Task::getName()
		{
			return this->_Name;
		}

		void Task::setName(std::string Name)
		{
			this->_Name = Name;
		}
	}
}