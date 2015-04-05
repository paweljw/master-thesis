#include "models/Task.hpp"
#include "utilhead.hpp"

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

		void Task::setCreated(std::string _Created)
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

		void Task::setUpdated(std::string _Updated)
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

		void Task::setCompleted(std::string _Completed)
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

		Task::Task(json_spirit::mValue v)
		{
			const json_spirit::mObject map = v.get_obj();

			this->setCompleted((*map.find("Completed")).second.get_str());
			this->setCreated((*map.find("Created")).second.get_str());
			this->setID((*map.find("ID")).second.get_int());
			this->setMetafile((*map.find("Metafile")).second.get_str());
			this->setName((*map.find("Name")).second.get_str());
			this->setNode((*map.find("Node")).second.get_str());
			this->setPartNum((*map.find("PartNum")).second.get_int());
			this->setState((*map.find("State")).second.get_int());
			this->setType((*map.find("Type")).second.get_int());
			this->setUpdated((*map.find("Updated")).second.get_str());
			this->setWave((*map.find("Wave")).second.get_int());
		}

		std::string Task::toJSONString()
		{
			json_spirit::Object taskobj;

			taskobj.push_back(json_spirit::Pair("Completed", horizon::sqlite3_time(this->getCompleted())));
			taskobj.push_back(json_spirit::Pair("Updated", horizon::sqlite3_time(this->getUpdated())));
			taskobj.push_back(json_spirit::Pair("Created", horizon::sqlite3_time(this->getCreated())));
			taskobj.push_back(json_spirit::Pair("ID", this->getID()));
			taskobj.push_back(json_spirit::Pair("Metafile", this->getMetafile()));
			taskobj.push_back(json_spirit::Pair("Name", this->getName()));
			taskobj.push_back(json_spirit::Pair("Node", this->getNode()));
			taskobj.push_back(json_spirit::Pair("PartNum", this->getPartNum()));
			taskobj.push_back(json_spirit::Pair("State", this->getState()));
			taskobj.push_back(json_spirit::Pair("Type", this->getType()));
			taskobj.push_back(json_spirit::Pair("Wave", this->getWave()));

			return json_spirit::write(taskobj, json_spirit::pretty_print);
		}

		json_spirit::Object Task::toJSONObject()
		{
			json_spirit::Object taskobj;

			taskobj.push_back(json_spirit::Pair("Completed", horizon::sqlite3_time(this->getCompleted())));
			taskobj.push_back(json_spirit::Pair("Updated", horizon::sqlite3_time(this->getUpdated())));
			taskobj.push_back(json_spirit::Pair("Created", horizon::sqlite3_time(this->getCreated())));
			taskobj.push_back(json_spirit::Pair("ID", this->getID()));
			taskobj.push_back(json_spirit::Pair("Metafile", this->getMetafile()));
			taskobj.push_back(json_spirit::Pair("Name", this->getName()));
			taskobj.push_back(json_spirit::Pair("Node", this->getNode()));
			taskobj.push_back(json_spirit::Pair("PartNum", this->getPartNum()));
			taskobj.push_back(json_spirit::Pair("State", this->getState()));
			taskobj.push_back(json_spirit::Pair("Type", this->getType()));
			taskobj.push_back(json_spirit::Pair("Wave", this->getWave()));

			return taskobj;
		}
	}
}
