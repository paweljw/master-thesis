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

#include "DatabaseAccessor.hpp"
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
	
	if (command == "screate" || command == "srecreate" || command == "sc")
	{
		horizon::db::DatabaseAccessor* dao = new horizon::db::ServerSQLiteDatabaseAccessor(database);
		dao->RecreateDatabase();
	}

	return 0;
}