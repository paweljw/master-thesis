#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// BOOST FILTERS
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include <boost/filesystem.hpp>

// UTILITIES
#include "utilhead.hpp"

#include "db/ServerDatabaseAccessor.hpp"
#include "db/ServerSQLiteDatabaseAccessor.hpp"

using namespace std;

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		cout << "Usage: dbctl file.db command" << endl;
		cout << "Commands: sc" << endl;
		return 0;
	}

	loggerhead("dbctl");

	using namespace logging::trivial;
	src::severity_logger< severity_level > lg;

	if(argc < 3)
	{
		BOOST_LOG_SEV(lg, fatal) << "Not enough parameters";
		return 1;
	}

	string database = argv[1];
	string command = argv[2];

	/*if (command == "create" || command == "recreate" || command == "c")
	{
		horizon::db::DatabaseAccessor* dao = new horizon::db::SQLiteDatabaseAccessor(database);
		dao->RecreateDatabase();
	}*/
	
	horizon::db::ServerDatabaseAccessor* dao = new horizon::db::ServerSQLiteDatabaseAccessor(database);

	if (command == "screate" || command == "srecreate" || command == "sc")
	{
		dao->RecreateDatabase();
	}

	if(command == "testSolution")
	{
		
	}

	if(command == "testWave")
	{
		horizon::models::Wave wave;
		wave.setSeq(100);
		wave.setTasks(10);
		dao->RegisterWave(wave);

		wave.decrementTasks();
		dao->UpdateWave(wave);

		horizon::models::Wave wave2;

		wave2.setID(wave.getID());

		dao->FillWave(wave2);

		if(wave.getTasks() == wave2.getTasks() && wave.getTasks() == 9)
			BOOST_LOG_SEV(lg, info) << "Wave Test complete, all ok.";
	}

	if(command == "testTask")
	{
		horizon::models::Task task;
		task.setName("Cokolwiek");
		task.setMetafile("this.mcx");
		task.setPartNum(8);
		dao->RegisterTask(task);

		task.setPartNum(7);
		dao->UpdateTask(task);

		horizon::models::Task task2(task.getID());

		dao->FillTask(task2);

		if(task.getPartNum() == task2.getPartNum() && task.getPartNum() == 7)
			BOOST_LOG_SEV(lg, info) << "Task Test complete, all ok.";
	}

	return 0;
}