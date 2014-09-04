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

#include "ServerDatabaseAccessor.hpp"
#include "ServerSQLiteDatabaseAccessor.hpp"

using namespace std;

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		cout << "Usage: dbctl file.s3db command" << endl;
		cout << "Commands: create" << endl;
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
		horizon::models::Solution solution;
		solution.setWave(1);
		solution.markStarted();
		solution.setName("testSolution");

		dao->RegisterSolution(solution);

		solution.setWave(5);
		dao->UpdateSolution(solution);

		horizon::models::Solution solution2;

		solution2.setID(solution.getID());

		dao->FillSolution(solution2);

		if(solution.getWave() == solution2.getWave())
			BOOST_LOG_SEV(lg, info) << "Solution Test complete, all ok.";
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

	return 0;
}